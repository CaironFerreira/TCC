#pragma once
#include <Arduino.h>

#include "control/TelemetryService.h"
#include "control/DisplayService.h"
#include "control/instruments/SpeedGauge.h"
#include "control/instruments/RpmGauge.h"
#include "control/instruments/FuelGauge.h"
#include "control/instruments/TireTempGauge.h"
#include "drivers/motors/GaugeMotorTmc2208.h"

struct AppConfig {
  const char* wifiSsid = nullptr;
  const char* wifiPass = nullptr;
  uint16_t udpPort = 0;
  uint32_t wifiConnectTimeoutMs = 15000;
};

class App {
public:
  App(TelemetryService& telemetry,
      DisplayService& ui,
      GaugeMotorTmc2208& speedMotor,
      GaugeMotorTmc2208& rpmMotor,
      GaugeMotorTmc2208& fuelMotor,
      GaugeMotorTmc2208& tempMotor,
      SpeedGauge& speedGauge,
      RpmGauge& rpmGauge,
      FuelGauge& fuelGauge,
      TireTempGauge& tireTempGauge);

  void begin(const AppConfig& cfg);
  void tick();

private:
  bool connectWifiWithTimeout();
  void updateUiWifiFields();
  void applyTelemetryToUi();

  void calibrateSpeedGauge();
  void calibrateRpmGauge();
  void calibrateFuelGauge();
  void calibrateTireTempGauge();

private:
  static const uint32_t UI_INTERVAL_MS = 100;
  static const uint32_t WIFI_INTERVAL_MS = 500;

  static const uint32_t FUEL_UPDATE_INTERVAL_MS = 30000;
  static const uint32_t TEMP_UPDATE_INTERVAL_MS = 5000;

  static const int32_t SPEED_CALIBRATION_BACKOFF_STEPS = 220;
  static const int32_t RPM_CALIBRATION_BACKOFF_STEPS = 220;
  static const int32_t FUEL_CALIBRATION_BACKOFF_STEPS = 220;
  static const int32_t TEMP_CALIBRATION_BACKOFF_STEPS = 220;

  TelemetryService& _telemetry;
  DisplayService& _ui;

  GaugeMotorTmc2208& _speedMotor;
  GaugeMotorTmc2208& _rpmMotor;
  GaugeMotorTmc2208& _fuelMotor;
  GaugeMotorTmc2208& _tempMotor;

  SpeedGauge& _speedGauge;
  RpmGauge& _rpmGauge;
  FuelGauge& _fuelGauge;
  TireTempGauge& _tireTempGauge;

  AppConfig _cfg;
  UiStatus _st;

  uint32_t _lastUiMs = 0;
  uint32_t _lastWifiMs = 0;
  uint32_t _lastFuelUpdateMs = 0;
  uint32_t _lastTempUpdateMs = 0;
};