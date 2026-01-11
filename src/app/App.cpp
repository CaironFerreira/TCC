// src/app/App.cpp
#include "App.h"
#include <WiFi.h>

App::App(TelemetryService& telemetry,
         DisplayService& ui,
         StepperL9110& speedMotor,
         SpeedGauge& speedGauge)
  : _telemetry(telemetry),
    _ui(ui),
    _speedMotor(speedMotor),
    _speedGauge(speedGauge) {}

void App::begin(const AppConfig& cfg) {
  _cfg = cfg;

  // OLED
  _ui.begin(_cfg.oledSda, _cfg.oledScl, _cfg.oledAddr);

  // estado inicial
  _st.ssid = _cfg.wifiSsid;
  _st.ip = "-";
  _st.wifiConnected = false;
  _st.speedKmh = 0.0f;
  _st.rpm = 0;
  _st.gear = 0;

  _ui.setStatus(_st);
  _ui.tick();

  // ===== Motor (velocímetro) =====
  // Inicia driver e faz homing por batente (curso todo pra esquerda)
  _speedMotor.begin(_cfg.speedMotorCfg);
  _speedMotor.homeByHardStop(_cfg.speedFullScaleMicrosteps);

  // Wi-Fi (conexão inicial com timeout)
  connectWifiWithTimeout();
  updateUiWifiFields();
  _ui.setStatus(_st);
  _ui.tick();

  // UDP / Telemetria
  _telemetry.begin(_cfg.udpPort);

  _lastUiMs = 0;
  _lastWifiMs = 0;
}

bool App::connectWifiWithTimeout() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(_cfg.wifiSsid, _cfg.wifiPass);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(150);

    updateUiWifiFields();
    _ui.setStatus(_st);
    _ui.tick();

    if (millis() - start > _cfg.wifiConnectTimeoutMs) {
      updateUiWifiFields();
      _ui.setStatus(_st);
      _ui.tick();
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
    _st.speedKmh = f.speedKmh;
    _st.rpm = f.rpm;
    _st.gear = f.gear;
  } else {
    _st.speedKmh = 0.0f;
    _st.rpm = 0;
    _st.gear = 0;
  }
}

void App::tick() {
  _telemetry.tick();

  const auto& f = _telemetry.lastFrame();

  _speedGauge.update(f);
  _speedMotor.tick();

  uint32_t now = millis();

  if (now - _lastWifiMs >= WIFI_INTERVAL_MS) {
    _lastWifiMs = now;
    updateUiWifiFields();
  }

  if (now - _lastUiMs < UI_INTERVAL_MS) return;
  _lastUiMs = now;

  applyTelemetryToUi();

  _ui.setStatus(_st);
  _ui.tick();
}
