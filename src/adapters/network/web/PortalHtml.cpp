#include "adapters/network/web/PortalHtml.h"

String PortalHtml::page(const char* deviceName) {
  const String safeDeviceName = htmlEscape(deviceName ? String(deviceName) : String("ESP32"));

  String content;
  content.reserve(1200);

  content += F(
    "<div class='badge'>Configuração Wi-Fi</div>"
    "<h1>Conectar dispositivo</h1>"
    "<p class='muted'>Dispositivo: <strong>"
  );
  content += safeDeviceName;
  content += F(
    "</strong></p>"
    "<p class='hint'>Informe o nome da rede e a senha para conectar a ESP32 à mesma rede local.</p>"
    "<form method='POST' action='/save' autocomplete='off'>"
      "<label for='ssid'>Nome da rede</label>"
      "<input id='ssid' name='ssid' type='text' maxlength='32' placeholder='Ex.: MinhaRede' required>"
      "<label for='password'>Senha</label>"
      "<div class='password-wrap'>"
        "<input id='password' name='password' type='password' maxlength='63' placeholder='Digite a senha'>"
        "<button class='password-toggle' type='button' aria-label='Mostrar senha' onclick='togglePassword(this)'>"
          "<svg viewBox='0 0 24 24' aria-hidden='true'>"
            "<path d='M2 12s3.5-6 10-6 10 6 10 6-3.5 6-10 6S2 12 2 12z'/>"
            "<circle cx='12' cy='12' r='3'/>"
          "</svg>"
        "</button>"
      "</div>"
      "<button type='submit'>Conectar</button>"
    "</form>"
  );

  return basePage("Configurar Wi-Fi", content);
}

String PortalHtml::errorPage(const char* deviceName, const String& ssid, const char* message) {
  const String safeDeviceName = htmlEscape(deviceName ? String(deviceName) : String("ESP32"));
  const String safeSsid = htmlEscape(ssid);
  const String safeMessage = htmlEscape(message ? String(message) : String("Falha na conexão."));

  String content;
  content.reserve(1400);

  content += F(
    "<div class='badge badge-error'>Erro</div>"
    "<h1>Falha na conexão</h1>"
    "<p class='muted'>Dispositivo: <strong>"
  );
  content += safeDeviceName;
  content += F(
    "</strong></p>"
    "<div class='msg error'>"
  );
  content += safeMessage;
  content += F(
    "</div>"
    "<p class='hint'>Verifique o nome da rede e a senha e tente novamente.</p>"
    "<form method='POST' action='/save' autocomplete='off'>"
      "<label for='ssid'>Nome da rede</label>"
      "<input id='ssid' name='ssid' type='text' maxlength='32' required value='"
  );
  content += safeSsid;
  content += F(
      "'>"
      "<label for='password'>Senha</label>"
      "<div class='password-wrap'>"
        "<input id='password' name='password' type='password' maxlength='63' placeholder='Digite a senha'>"
        "<button class='password-toggle' type='button' aria-label='Mostrar senha' onclick='togglePassword(this)'>"
          "<svg viewBox='0 0 24 24' aria-hidden='true'>"
            "<path d='M2 12s3.5-6 10-6 10 6 10 6-3.5 6-10 6S2 12 2 12z'/>"
            "<circle cx='12' cy='12' r='3'/>"
          "</svg>"
        "</button>"
      "</div>"
      "<button type='submit'>Tentar novamente</button>"
    "</form>"
  );

  return basePage("Falha na conexão", content);
}

String PortalHtml::successPage(const String& ip) {
  const String safeIp = htmlEscape(ip);

  String content;
  content.reserve(700);

  content += F(
    "<div class='badge badge-ok'>Conectado</div>"
    "<h1>Conexão realizada</h1>"
    "<div class='msg ok'>Wi-Fi conectado com sucesso.</div>"
    "<p class='muted'>IP local do dispositivo</p>"
    "<div class='ip'>"
  );
  content += safeIp;
  content += F(
    "</div>"
    "<p class='hint'>O portal será encerrado em alguns segundos.</p>"
  );

  return basePage("Wi-Fi conectado", content);
}

String PortalHtml::basePage(const String& title, const String& content) {
  String html;
  html.reserve(3200);

  html += F(
    "<!DOCTYPE html><html lang='pt-BR'><head>"
    "<meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<meta http-equiv='Cache-Control' content='no-cache, no-store, must-revalidate'>"
    "<meta http-equiv='Pragma' content='no-cache'>"
    "<meta http-equiv='Expires' content='0'>"
    "<title>"
  );
  html += htmlEscape(title);
  html += F(
    "</title>"
    "<style>"
      "*{box-sizing:border-box}"
      "body{margin:0;min-height:100vh;display:flex;align-items:center;justify-content:center;"
      "padding:18px;background:linear-gradient(180deg,#081018 0%,#0d1721 100%);"
      "font-family:Arial,Helvetica,sans-serif;color:#eef6ff}"
      ".card{width:100%;max-width:380px;background:#111c27;border:1px solid #22384a;"
      "border-radius:18px;padding:22px;box-shadow:0 10px 30px rgba(0,0,0,.28)}"
      ".badge{display:inline-block;padding:6px 10px;border-radius:999px;font-size:12px;font-weight:700;"
      "background:#143049;color:#9ed0ff;margin-bottom:14px}"
      ".badge-ok{background:#153626;color:#b8ffd7}"
      ".badge-error{background:#3a1a1a;color:#ffbaba}"
      "h1{margin:0 0 10px;font-size:24px;line-height:1.2}"
      ".muted{margin:0 0 10px;color:#a8bfd2;font-size:14px}"
      ".hint{margin:0 0 16px;color:#8fa6b9;font-size:14px;line-height:1.5}"
      "label{display:block;margin:14px 0 6px;font-size:14px;color:#dce9f6;font-weight:600}"
      "input{width:100%;padding:12px 13px;border:1px solid #2a4255;border-radius:12px;"
      "background:#0b141c;color:#fff;font-size:16px;outline:none}"
      "input:focus{border-color:#4aa3ff}"
      ".password-wrap{position:relative}"
      ".password-wrap input{padding-right:48px}"
      "button{width:100%;margin-top:16px;padding:12px 14px;border:0;border-radius:12px;"
      "background:#2d8cff;color:#fff;font-size:16px;font-weight:700;cursor:pointer}"
      ".password-toggle{position:absolute;right:5px;top:50%;transform:translateY(-50%);"
      "width:38px;height:38px;margin:0;padding:0;background:transparent;color:#a8bfd2;"
      "line-height:1;border-radius:8px;display:flex;align-items:center;justify-content:center}"
      ".password-toggle svg{width:21px;height:21px;fill:none;stroke:currentColor;"
      "stroke-width:2;stroke-linecap:round;stroke-linejoin:round}"
      ".password-toggle:focus{outline:1px solid #4aa3ff}"
      ".msg{padding:12px 14px;border-radius:12px;margin:0 0 14px;font-size:14px;line-height:1.4}"
      ".ok{background:#163525;border:1px solid #24563a;color:#d9ffe8}"
      ".error{background:#3a1717;border:1px solid #6d2c2c;color:#ffdede}"
      ".ip{padding:14px;border-radius:12px;background:#0d2232;border:1px solid #27465e;"
      "font-size:20px;font-weight:700;letter-spacing:.4px;word-break:break-word}"
      "@media (max-width:420px){"
        ".card{padding:18px;border-radius:16px}"
        "h1{font-size:22px}"
      "}"
    "</style>"
    "</head><body><div class='card'>"
  );

  html += content;
  html += F(
    "</div>"
    "<script>"
      "function togglePassword(btn){"
        "var input=btn.parentNode.querySelector('input');"
        "if(!input)return;"
        "var visible=input.type==='text';"
        "input.type=visible?'password':'text';"
        "btn.setAttribute('aria-label',visible?'Mostrar senha':'Ocultar senha');"
      "}"
    "</script>"
    "</body></html>"
  );

  return html;
}

String PortalHtml::htmlEscape(const String& value) {
  String out;
  out.reserve(value.length() + 16);

  for (size_t i = 0; i < value.length(); ++i) {
    const char c = value[i];
    switch (c) {
      case '&':  out += F("&amp;");  break;
      case '<':  out += F("&lt;");   break;
      case '>':  out += F("&gt;");   break;
      case '\"': out += F("&quot;"); break;
      case '\'': out += F("&#39;");  break;
      default:   out += c;           break;
    }
  }

  return out;
}
