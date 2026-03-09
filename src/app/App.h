#pragma once
#include <Arduino.h>

#include "control/TelemetryService.h"
#include "control/DisplayService.h"
#include "control/instruments/SpeedGauge.h"
#include "control/instruments/RpmGauge.h"
#include "drivers/motors/GaugeMotorTmc2208.h"

struct AppConfig {
  const char* wifiSsid = nullptr;
  const char* wifiPass = nullptr;
  uint16_t udpPort = 0;
  uint32_t wifiConnectTimeoutMs = 15000;

  int oledSda = -1;
  int oledScl = -1;
  uint8_t oledAddr = 0x3C;
};

class App {
public:
  App(TelemetryService& telemetry,
      DisplayService& ui,
      GaugeMotorTmc2208& speedMotor,
      GaugeMotorTmc2208& rpmMotor,
      SpeedGauge& speedGauge,
      RpmGauge& rpmGauge);

  void begin(const AppConfig& cfg);
  void tick();

private:
  bool connectWifiWithTimeout();
  void updateUiWifiFields();
  void applyTelemetryToUi();

  void calibrateSpeedGauge();
  void calibrateRpmGauge();

private:
  static const uint32_t UI_INTERVAL_MS = 100;
  static const uint32_t WIFI_INTERVAL_MS = 500;

  static const int32_t SPEED_CALIBRATION_BACKOFF_STEPS = 220;
  static const int32_t RPM_CALIBRATION_BACKOFF_STEPS = 220;

  TelemetryService& _telemetry;
  DisplayService& _ui;

  GaugeMotorTmc2208& _speedMotor;
  GaugeMotorTmc2208& _rpmMotor;

  SpeedGauge& _speedGauge;
  RpmGauge& _rpmGauge;

  AppConfig _cfg;
  UiStatus _st;

  uint32_t _lastUiMs = 0;
  uint32_t _lastWifiMs = 0;
};