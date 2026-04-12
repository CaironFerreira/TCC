#include "WiFiConfigPortal.h"
#include "../web/PortalHtml.h"

WiFiConfigPortal::WiFiConfigPortal()
: _cfg(), _server(80) {}

WiFiConfigPortal::WiFiConfigPortal(const Config& cfg)
: _cfg(cfg), _server(80) {}

bool WiFiConfigPortal::begin() {
  configureWifiBase();
  loadCredentials();

  if (!_ssid.isEmpty()) {
    const ConnectResult result = tryConnect();
    if (result == ConnectResult::Success) {
      _portalActive = false;
      _portalStopPending = false;
      _state = PortalState::Success;
      _lastResult = ConnectResult::Success;
      _lastStatusMessage = connectResultMessage(ConnectResult::Success);
      return true;
    }
  }

  startPortal();
  return false;
}

void WiFiConfigPortal::tick() {
  if (_portalActive) {
    _server.handleClient();
  }

  if (_connectRequested) {
    _connectRequested = false;
    beginConnectionAttempt(_pendingSsid, _pendingPassword);
  }

  if (_state == PortalState::Connecting) {
    processConnectionAttempt();
  }

  if (_portalStopPending && (millis() - _successAtMs >= PORTAL_CLOSE_DELAY_MS)) {
    _portalStopPending = false;
    stopPortal();
  }
}

bool WiFiConfigPortal::isConnected() const {
  return WiFi.status() == WL_CONNECTED;
}

bool WiFiConfigPortal::isPortalActive() const {
  return _portalActive;
}

String WiFiConfigPortal::localIp() const {
  if (WiFi.status() == WL_CONNECTED) {
    return WiFi.localIP().toString();
  }

  if (_portalActive) {
    return WiFi.softAPIP().toString();
  }

  return "0.0.0.0";
}

String WiFiConfigPortal::connectedSsid() const {
  return WiFi.SSID();
}

String WiFiConfigPortal::lastStatusMessage() const {
  return _lastStatusMessage;
}

String WiFiConfigPortal::connectionState() const {
  switch (_state) {
    case PortalState::Idle:
      return "idle";
    case PortalState::Connecting:
      return "connecting";
    case PortalState::Success:
      return "success";
    case PortalState::Error:
    default:
      return "error";
  }
}

void WiFiConfigPortal::clearCredentials() {
  _prefs.begin("wifi", false);
  _prefs.clear();
  _prefs.end();

  _ssid = "";
  _password = "";
  _pendingSsid = "";
  _pendingPassword = "";
  _lastResult = ConnectResult::Failed;
  _state = PortalState::Idle;
  _lastStatusMessage = "Credenciais apagadas.";
}

void WiFiConfigPortal::loadCredentials() {
  _prefs.begin("wifi", true);
  _ssid = _prefs.getString("ssid", "");
  _password = _prefs.getString("password", "");
  _prefs.end();
}

void WiFiConfigPortal::saveCredentials(const String& ssid, const String& password) {
  _prefs.begin("wifi", false);
  _prefs.putString("ssid", ssid);
  _prefs.putString("password", password);
  _prefs.end();
}

WiFiConfigPortal::ConnectResult WiFiConfigPortal::tryConnect() {
  if (!isValidSsid(_ssid)) {
    _lastStatusMessage = connectResultMessage(ConnectResult::EmptySsid);
    return ConnectResult::EmptySsid;
  }

  if (!isValidPassword(_password)) {
    _lastStatusMessage = connectResultMessage(ConnectResult::InvalidPassword);
    return ConnectResult::InvalidPassword;
  }

  configureWifiBase();

  WiFi.mode(WIFI_STA);
  delay(100);

  WiFi.disconnect(false, true);
  delay(100);

  WiFi.begin(_ssid.c_str(), _password.c_str());

  const uint32_t start = millis();

  while ((millis() - start) < _cfg.connectTimeoutMs) {
    const wl_status_t status = WiFi.status();

    if (status == WL_CONNECTED) {
      _lastStatusMessage = connectResultMessage(ConnectResult::Success);
      return ConnectResult::Success;
    }

    if (status == WL_CONNECT_FAILED) {
      WiFi.disconnect(false, true);
      _lastStatusMessage = connectResultMessage(ConnectResult::AuthFailed);
      return ConnectResult::AuthFailed;
    }

    delay(100);
  }

  WiFi.disconnect(false, true);
  _lastStatusMessage = connectResultMessage(ConnectResult::Timeout);
  return ConnectResult::Timeout;
}

void WiFiConfigPortal::beginConnectionAttempt(const String& ssid, const String& password) {
  if (!isValidSsid(ssid)) {
    _lastResult = ConnectResult::EmptySsid;
    _lastStatusMessage = connectResultMessage(_lastResult);
    _state = PortalState::Error;
    return;
  }

  if (!isValidPassword(password)) {
    _lastResult = ConnectResult::InvalidPassword;
    _lastStatusMessage = connectResultMessage(_lastResult);
    _state = PortalState::Error;
    return;
  }

  _pendingSsid = ssid;
  _pendingPassword = password;
  _state = PortalState::Connecting;
  _lastResult = ConnectResult::Failed;
  _lastStatusMessage = "Conectando...";
  _portalStopPending = false;

  configureWifiBase();

  WiFi.mode(WIFI_AP_STA);
  delay(100);

  WiFi.disconnect(false, true);
  delay(100);

  WiFi.begin(_pendingSsid.c_str(), _pendingPassword.c_str());
  _connectStartMs = millis();
}

void WiFiConfigPortal::processConnectionAttempt() {
  const wl_status_t status = WiFi.status();

  if (status == WL_CONNECTED) {
    saveCredentials(_pendingSsid, _pendingPassword);
    _ssid = _pendingSsid;
    _password = _pendingPassword;

    _lastResult = ConnectResult::Success;
    _lastStatusMessage = connectResultMessage(_lastResult);
    _state = PortalState::Success;

    _successAtMs = millis();
    _portalStopPending = true;
    return;
  }

  if (status == WL_CONNECT_FAILED) {
    WiFi.disconnect(false, true);
    _lastResult = ConnectResult::AuthFailed;
    _lastStatusMessage = connectResultMessage(_lastResult);
    _state = PortalState::Error;
    return;
  }

  if ((millis() - _connectStartMs) >= _cfg.connectTimeoutMs) {
    WiFi.disconnect(false, true);
    _lastResult = ConnectResult::Timeout;
    _lastStatusMessage = connectResultMessage(_lastResult);
    _state = PortalState::Error;
    return;
  }
}

void WiFiConfigPortal::startPortal() {
  configureWifiBase();

  WiFi.mode(WIFI_AP);
  delay(100);

  const bool ok = WiFi.softAP(_cfg.apSsid, _cfg.apPassword);
  if (!ok) {
    _portalActive = false;
    _lastStatusMessage = "Falha ao iniciar portal.";
    return;
  }

  setupRoutes();
  _server.begin();

  _portalActive = true;
  _portalStopPending = false;
  _state = PortalState::Idle;
  _lastResult = ConnectResult::Failed;
  _lastStatusMessage = "Portal ativo.";
}

void WiFiConfigPortal::stopPortal() {
  _server.stop();
  delay(50);

  WiFi.softAPdisconnect(true);
  delay(50);

  WiFi.mode(WIFI_STA);
  delay(50);

  _portalActive = false;
}

void WiFiConfigPortal::setupRoutes() {
  _server.on("/", HTTP_GET, [this]() { handleRoot(); });
  _server.on("/save", HTTP_POST, [this]() { handleSave(); });
  _server.on("/status", HTTP_GET, [this]() { handleStatus(); });
  _server.onNotFound([this]() { handleNotFound(); });
}

void WiFiConfigPortal::handleRoot() {
  _server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  _server.sendHeader("Pragma", "no-cache");
  _server.sendHeader("Expires", "0");
  _server.send(200, "text/html; charset=utf-8", PortalHtml::page(_cfg.apSsid));
}

void WiFiConfigPortal::handleSave() {
  String ssid = _server.arg("ssid");
  String password = _server.arg("password");

  ssid.trim();
  password.trim();

  if (!isValidSsid(ssid)) {
    _lastResult = ConnectResult::EmptySsid;
    _lastStatusMessage = connectResultMessage(_lastResult);
    _state = PortalState::Error;
    _server.send(400, "text/html; charset=utf-8",
                 PortalHtml::errorPage(_cfg.apSsid, ssid, _lastStatusMessage.c_str()));
    return;
  }

  if (!isValidPassword(password)) {
    _lastResult = ConnectResult::InvalidPassword;
    _lastStatusMessage = connectResultMessage(_lastResult);
    _state = PortalState::Error;
    _server.send(400, "text/html; charset=utf-8",
                 PortalHtml::errorPage(_cfg.apSsid, ssid, _lastStatusMessage.c_str()));
    return;
  }

  _pendingSsid = ssid;
  _pendingPassword = password;
  _connectRequested = true;
  _state = PortalState::Connecting;
  _lastResult = ConnectResult::Failed;
  _lastStatusMessage = "Conectando...";

  String html;
  html.reserve(1500);
  html += F(
    "<!DOCTYPE html><html lang='pt-BR'><head>"
    "<meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>Conectando</title>"
    "<style>"
    "*{box-sizing:border-box}"
    "body{margin:0;min-height:100vh;display:flex;align-items:center;justify-content:center;padding:18px;"
    "background:linear-gradient(180deg,#081018 0%,#0d1721 100%);font-family:Arial,Helvetica,sans-serif;color:#eef6ff}"
    ".card{width:100%;max-width:380px;background:#111c27;border:1px solid #22384a;border-radius:18px;padding:22px;"
    "box-shadow:0 10px 30px rgba(0,0,0,.28)}"
    ".badge{display:inline-block;padding:6px 10px;border-radius:999px;font-size:12px;font-weight:700;"
    "background:#143049;color:#9ed0ff;margin-bottom:14px}"
    "h1{margin:0 0 10px;font-size:24px;line-height:1.2}"
    "p{margin:0;color:#a8bfd2;font-size:14px;line-height:1.5}"
    "</style></head><body><div class='card'>"
    "<div class='badge'>Conectando</div>"
    "<h1 id='title'>Tentando conectar</h1>"
    "<p id='msg'>Aguarde alguns segundos enquanto a ESP32 valida a rede.</p>"
    "</div>"
    "<script>"
    "async function poll(){"
      "try{"
        "const r=await fetch('/status',{cache:'no-store'});"
        "const j=await r.json();"
        "if(j.state==='connecting'){setTimeout(poll,700);return;}"
        "if(j.state==='success'){"
          "document.getElementById('title').textContent='Conectado com sucesso';"
          "document.getElementById('msg').textContent='IP: '+j.ip+' . O portal será encerrado em alguns segundos.';"
          "return;"
        "}"
        "document.getElementById('title').textContent='Falha na conexão';"
        "document.getElementById('msg').textContent='Nome da rede ou senha inválidos, ou a rede não respondeu.';"
      "}catch(e){setTimeout(poll,1000);}"
    "}"
    "poll();"
    "</script></body></html>"
  );

  _server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  _server.sendHeader("Pragma", "no-cache");
  _server.sendHeader("Expires", "0");
  _server.send(200, "text/html; charset=utf-8", html);
}

void WiFiConfigPortal::handleStatus() {
  String json;
  json.reserve(160);
  json += "{\"state\":\"";
  json += connectionState();
  json += "\",\"message\":\"";
  json += String(connectResultMessage(_lastResult));
  json += "\",\"ip\":\"";
  json += WiFi.localIP().toString();
  json += "\"}";

  _server.send(200, "application/json; charset=utf-8", json);
}

void WiFiConfigPortal::handleNotFound() {
  _server.sendHeader("Location", "/", true);
  _server.send(302, "text/plain", "");
}

void WiFiConfigPortal::configureWifiBase() {
  WiFi.persistent(false);
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(false);
}

const char* WiFiConfigPortal::connectResultMessage(ConnectResult result) const {
  switch (result) {
    case ConnectResult::Success:
      return "Wi-Fi conectado com sucesso.";
    case ConnectResult::EmptySsid:
      return "Digite o nome da rede Wi-Fi.";
    case ConnectResult::InvalidPassword:
      return "Senha inválida.";
    case ConnectResult::Timeout:
      return "Falha na conexão.";
    case ConnectResult::AuthFailed:
      return "Falha na conexão.";
    case ConnectResult::Failed:
    default:
      return "Falha na conexão.";
  }
}

bool WiFiConfigPortal::isValidSsid(const String& ssid) const {
  return !ssid.isEmpty() && ssid.length() <= 32;
}

bool WiFiConfigPortal::isValidPassword(const String& password) const {
  if (password.isEmpty()) {
    return true;
  }

  return password.length() >= 8 && password.length() <= 63;
}