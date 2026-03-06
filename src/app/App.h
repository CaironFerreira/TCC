#pragma once
#include <Arduino.h>

#include "control/TelemetryService.h"
#include "control/DisplayService.h"
#include "control/instruments/SpeedGauge.h"
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
      SpeedGauge& speedGauge);

  void begin(const AppConfig& cfg);
  void tick();

private:
  void updateUiWifiFields();
  void applyTelemetryToUi();
  bool connectWifiWithTimeout();

  // calibração do ponteiro no boot
  void calibrateSpeedGauge();

  TelemetryService& _telemetry;
  DisplayService& _ui;
  GaugeMotorTmc2208& _speedMotor;
  SpeedGauge& _speedGauge;

  AppConfig _cfg{};
  UiStatus _st{};

  uint32_t _lastUiMs = 0;
  uint32_t _lastWifiMs = 0;

  // Intervalos
  static constexpr uint32_t UI_INTERVAL_MS   = 33;   // ~30 FPS
  static constexpr uint32_t WIFI_INTERVAL_MS = 500;  // status a cada 0,5s

  // calibração no boot
  static constexpr int32_t SPEED_CALIBRATION_BACKOFF_STEPS = 200;
};