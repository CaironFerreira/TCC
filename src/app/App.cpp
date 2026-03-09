#include "App.h"
#include <WiFi.h>
#include <math.h>

App::App(TelemetryService& telemetry,
         DisplayService& ui,
         GaugeMotorTmc2208& speedMotor,
         GaugeMotorTmc2208& rpmMotor,
         SpeedGauge& speedGauge,
         RpmGauge& rpmGauge)
  : _telemetry(telemetry),
    _ui(ui),
    _speedMotor(speedMotor),
    _rpmMotor(rpmMotor),
    _speedGauge(speedGauge),
    _rpmGauge(rpmGauge) {}

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

  _ui.begin(_cfg.oledSda, _cfg.oledScl, _cfg.oledAddr);

  _speedMotor.begin();
  _rpmMotor.begin();

  calibrateSpeedGauge();
  calibrateRpmGauge();

  _speedGauge.begin();
  _rpmGauge.begin();

  connectWifiWithTimeout();

  _telemetry.begin(_cfg.udpPort);

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

    uint32_t now = micros();
    _speedGauge.tick(now);
    _rpmGauge.tick(now);

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
    _st.rpm = (int)f.rpm;
    _st.gear = f.gear;
  } else {
    _st.speedKmh = 0.0f;
    _st.rpm = 0;
    _st.gear = 0;
  }
}

void App::calibrateSpeedGauge() {
  _speedMotor.setCurrentSteps(0);
  _speedMotor.moveTo(-SPEED_CALIBRATION_BACKOFF_STEPS);

  while (_speedMotor.isMoving()) {
    _speedMotor.tick(micros());
  }

  _speedMotor.setCurrentSteps(0);
  _speedMotor.moveTo(0);
}

void App::calibrateRpmGauge() {
  _rpmMotor.setCurrentSteps(0);
  _rpmMotor.moveTo(-RPM_CALIBRATION_BACKOFF_STEPS);

  while (_rpmMotor.isMoving()) {
    _rpmMotor.tick(micros());
  }

  _rpmMotor.setCurrentSteps(0);
  _rpmMotor.moveTo(0);
}

void App::tick() {
  _telemetry.tick();

  const auto& f = _telemetry.lastFrame();

  float speedKmh = 0.0f;
  float rpm = 0.0f;

  if (f.valid) {
    if (isfinite(f.speedKmh) && f.speedKmh > 0.0f)
      speedKmh = f.speedKmh;

    if (isfinite(f.rpm) && f.rpm > 0.0f)
      rpm = f.rpm;
  }

  _speedGauge.setSpeedKmh(speedKmh);
  _rpmGauge.setRpm(rpm);

  uint32_t nowMicros = micros();

  _speedGauge.tick(nowMicros);
  _rpmGauge.tick(nowMicros);

  const uint32_t now = millis();

  if (now - _lastWifiMs >= WIFI_INTERVAL_MS) {
    _lastWifiMs = now;
    updateUiWifiFields();
  }

  if (now - _lastUiMs >= UI_INTERVAL_MS) {
    _lastUiMs = now;

    applyTelemetryToUi();
    _ui.setStatus(_st);
    _ui.tick();
  }
}