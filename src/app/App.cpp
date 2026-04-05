#include "App.h"
#include <WiFi.h>
#include <math.h>

static float fahrenheitToCelsius(float f) {
  return (f - 32.0f) * 5.0f / 9.0f;
}

static constexpr float CALIBRATION_RISE_STEPS_PER_SEC = 300.0f;
static constexpr float CALIBRATION_FALL_STEPS_PER_SEC = 300.0f;

App::App(TelemetryService& telemetry,
         DisplayService& ui,
         GaugeMotorTmc2208& speedMotor,
         GaugeMotorTmc2208& rpmMotor,
         GaugeMotorTmc2208& fuelMotor,
         GaugeMotorTmc2208& tempMotor,
         SpeedGauge& speedGauge,
         RpmGauge& rpmGauge,
         FuelGauge& fuelGauge,
         TireTempGauge& tireTempGauge)
  : _telemetry(telemetry),
    _ui(ui),
    _speedMotor(speedMotor),
    _rpmMotor(rpmMotor),
    _fuelMotor(fuelMotor),
    _tempMotor(tempMotor),
    _speedGauge(speedGauge),
    _rpmGauge(rpmGauge),
    _fuelGauge(fuelGauge),
    _tireTempGauge(tireTempGauge) {}

void App::begin(const AppConfig& cfg) {
  _cfg = cfg;

  _st.ssid = _cfg.wifiSsid;
  _st.ip = "-";
  _st.wifiConnected = false;
  _st.speedKmh = 0.0f;
  _st.rpm = 0;
  _st.gear = 0;
  _st.fuel = 0.0f;
  _st.tireTempAvg = 0.0f;

  _lastUiMs = 0;
  _lastWifiMs = 0;
  _lastFuelUpdateMs = 0;
  _lastTempUpdateMs = 0;

  _ui.begin();

  _speedMotor.begin();
  _rpmMotor.begin();
  _fuelMotor.begin();
  _tempMotor.begin();

  calibrateSpeedGauge();
  calibrateRpmGauge();
  calibrateFuelGauge();
  calibrateTireTempGauge();

  _speedGauge.begin();
  _rpmGauge.begin();
  _fuelGauge.begin();
  _tireTempGauge.begin();

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

  const uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(150);

    const uint32_t nowMicros = micros();
    _speedGauge.tick(nowMicros);
    _rpmGauge.tick(nowMicros);
    _fuelGauge.tick(nowMicros);
    _tireTempGauge.tick(nowMicros);

    if (millis() - start > _cfg.wifiConnectTimeoutMs) {
      return false;
    }
  }

  return true;
}

void App::updateUiWifiFields() {
  const wl_status_t wst = WiFi.status();
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
    _st.speedKmh = (isfinite(f.speedKmh) && f.speedKmh > 0.0f) ? f.speedKmh : 0.0f;
    _st.rpm = (isfinite(f.rpm) && f.rpm > 0.0f) ? f.rpm : 0.0f;
    _st.gear = f.gear;
    _st.fuel = (isfinite(f.fuel) && f.fuel >= 0.0f) ? f.fuel : 0.0f;
    _st.tireTempAvg = isfinite(f.tireTempAvg) ? fahrenheitToCelsius(f.tireTempAvg) : 0.0f;
  } else {
    _st.speedKmh = 0.0f;
    _st.rpm = 0;
    _st.gear = 0;
    _st.fuel = 0.0f;
    _st.tireTempAvg = 0.0f;
  }
}

void App::calibrateSpeedGauge() {
  _speedMotor.setCurrentPosition(0);
  _speedMotor.moveTo(
    -SPEED_CALIBRATION_BACKOFF_STEPS,
    CALIBRATION_RISE_STEPS_PER_SEC,
    CALIBRATION_FALL_STEPS_PER_SEC
  );

  while (_speedMotor.isMoving()) {
    _speedMotor.tick(micros());
  }

  _speedMotor.setCurrentPosition(0);
  _speedMotor.moveTo(
    0,
    CALIBRATION_RISE_STEPS_PER_SEC,
    CALIBRATION_FALL_STEPS_PER_SEC
  );
}

void App::calibrateRpmGauge() {
  _rpmMotor.setCurrentPosition(0);
  _rpmMotor.moveTo(
    -RPM_CALIBRATION_BACKOFF_STEPS,
    CALIBRATION_RISE_STEPS_PER_SEC,
    CALIBRATION_FALL_STEPS_PER_SEC
  );

  while (_rpmMotor.isMoving()) {
    _rpmMotor.tick(micros());
  }

  _rpmMotor.setCurrentPosition(0);
  _rpmMotor.moveTo(
    0,
    CALIBRATION_RISE_STEPS_PER_SEC,
    CALIBRATION_FALL_STEPS_PER_SEC
  );
}

void App::calibrateFuelGauge() {
  _fuelMotor.setCurrentPosition(0);
  _fuelMotor.moveTo(
    -FUEL_CALIBRATION_BACKOFF_STEPS,
    CALIBRATION_RISE_STEPS_PER_SEC,
    CALIBRATION_FALL_STEPS_PER_SEC
  );

  while (_fuelMotor.isMoving()) {
    _fuelMotor.tick(micros());
  }

  _fuelMotor.setCurrentPosition(0);
  _fuelMotor.moveTo(
    0,
    CALIBRATION_RISE_STEPS_PER_SEC,
    CALIBRATION_FALL_STEPS_PER_SEC
  );
}

void App::calibrateTireTempGauge() {
  _tempMotor.setCurrentPosition(0);
  _tempMotor.moveTo(
    -TEMP_CALIBRATION_BACKOFF_STEPS,
    CALIBRATION_RISE_STEPS_PER_SEC,
    CALIBRATION_FALL_STEPS_PER_SEC
  );

  while (_tempMotor.isMoving()) {
    _tempMotor.tick(micros());
  }

  _tempMotor.setCurrentPosition(0);
  _tempMotor.moveTo(
    0,
    CALIBRATION_RISE_STEPS_PER_SEC,
    CALIBRATION_FALL_STEPS_PER_SEC
  );
}

void App::tick() {
  _telemetry.tick();

  const auto& f = _telemetry.lastFrame();
  const uint32_t now = millis();
  const uint32_t nowMicros = micros();

  float speedKmh = 0.0f;
  float rpm = 0.0f;

  if (f.valid) {
    if (isfinite(f.speedKmh) && f.speedKmh > 0.0f) {
      speedKmh = f.speedKmh;
    }

    if (isfinite(f.rpm) && f.rpm > 0.0f) {
      rpm = f.rpm;
    }
  }

  _speedGauge.setSpeedKmh(speedKmh);
  _rpmGauge.setRpm(rpm);

  if (now - _lastFuelUpdateMs >= FUEL_UPDATE_INTERVAL_MS) {
    _lastFuelUpdateMs = now;

    float fuel = 0.0f;
    if (f.valid && isfinite(f.fuel) && f.fuel >= 0.0f) {
      fuel = f.fuel;
    }

    _fuelGauge.setFuelLevel(fuel);
  }

  if (now - _lastTempUpdateMs >= TEMP_UPDATE_INTERVAL_MS) {
    _lastTempUpdateMs = now;

    float tireTempC = 0.0f;
    if (f.valid && isfinite(f.tireTempAvg)) {
      tireTempC = fahrenheitToCelsius(f.tireTempAvg);
    }

    _tireTempGauge.setTemperature(tireTempC);
  }

  _speedGauge.tick(nowMicros);
  _rpmGauge.tick(nowMicros);
  _fuelGauge.tick(nowMicros);
  _tireTempGauge.tick(nowMicros);

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