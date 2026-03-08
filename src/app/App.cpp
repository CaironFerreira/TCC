#include "App.h"
#include <WiFi.h>
#include <math.h>

App::App(TelemetryService& telemetry,
         DisplayService& ui,
         GaugeMotorTmc2208& speedMotor,
         SpeedGauge& speedGauge)
  : _telemetry(telemetry),
    _ui(ui),
    _speedMotor(speedMotor),
    _speedGauge(speedGauge) {}

void App::begin(const AppConfig& cfg) {
  _cfg = cfg;

  _st.ssid = _cfg.wifiSsid;
  _st.ip = "-";
  _st.wifiConnected = false;
  _st.speedKmh = 0.0f;
  _st.rpm = 0;
  _st.gear = 0;

  _lastUiMs = 0;
  _lastWifiMs = 0;

  // Inicializa o display
  _ui.begin(_cfg.oledSda, _cfg.oledScl, _cfg.oledAddr);

  // Inicializa o motor e calibra o ponteiro
  _speedMotor.begin();
  calibrateSpeedGauge();

  // Inicializa o gauge já com o ponteiro em 0 km/h
  _speedGauge.begin();

  // Conecta no Wi-Fi
  connectWifiWithTimeout();

  // Inicia a telemetria UDP
  _telemetry.begin(_cfg.udpPort);

  // Atualiza UI inicial
  updateUiWifiFields();
  applyTelemetryToUi();
  _ui.setStatus(_st);
  _ui.tick();
}

bool App::connectWifiWithTimeout() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(_cfg.wifiSsid, _cfg.wifiPass);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(150);

    // mantém o motor responsivo mesmo durante a conexão
    _speedGauge.tick(micros());

    if (millis() - start > _cfg.wifiConnectTimeoutMs) {
      return false;
    }
  }

  return true;
}

void App::updateUiWifiFields() {
  wl_status_t wst = WiFi.status();
  _st.wifiConnected = (wst == WL_CONNECTED);

  if (_st.wifiConnected) {
    _st.ssid = WiFi.SSID();
    _st.ip = WiFi.localIP().toString();
  } else {
    _st.ssid = _cfg.wifiSsid;
    _st.ip = "-";
  }
}

void App::applyTelemetryToUi() {
  const auto& f = _telemetry.lastFrame();

  if (f.valid) {
    _st.speedKmh = isfinite(f.speedKmh) && f.speedKmh > 0.0f ? f.speedKmh : 0.0f;
    _st.rpm = f.rpm;
    _st.gear = f.gear;
  } else {
    _st.speedKmh = 0.0f;
    _st.rpm = 0;
    _st.gear = 0;
  }
}

void App::calibrateSpeedGauge() {
  // Anda para trás além do necessário para garantir chegada ao batente.
  _speedMotor.setCurrentSteps(0);
  _speedMotor.moveTo(-SPEED_CALIBRATION_BACKOFF_STEPS);

  while (_speedMotor.isMoving()) {
    _speedMotor.tick(micros());
  }

  // Ao encostar no batente, assume posição zero.
  _speedMotor.setCurrentSteps(0);
  _speedMotor.moveTo(0);
}

void App::tick() {
  // Atualiza telemetria
  _telemetry.tick();

  // Atualiza gauge a partir da última telemetria
  const auto& f = _telemetry.lastFrame();

  float speedKmh = 0.0f;
  if (f.valid && isfinite(f.speedKmh) && f.speedKmh > 0.0f) {
    speedKmh = f.speedKmh;
  }

  _speedGauge.setSpeedKmh(speedKmh);
  _speedGauge.tick(micros());

  const uint32_t now = millis();

  // Atualiza estado de Wi-Fi em intervalos
  if (now - _lastWifiMs >= WIFI_INTERVAL_MS) {
    _lastWifiMs = now;
    updateUiWifiFields();
  }

  // Atualiza dados da UI em intervalos
  if (now - _lastUiMs >= UI_INTERVAL_MS) {
    _lastUiMs = now;

    applyTelemetryToUi();
    _ui.setStatus(_st);
    _ui.tick();
  }
}