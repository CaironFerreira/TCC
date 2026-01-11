#pragma once
#include <Arduino.h>

#include "control/TelemetryService.h"
#include "control/DisplayService.h"
#include "drivers/motors/StepperL9110.h"
#include "control/instruments/SpeedGauge.h"

struct AppConfig {
  const char* wifiSsid;
  const char* wifiPass;
  uint16_t udpPort;
  uint32_t wifiConnectTimeoutMs;

  int oledSda;
  int oledScl;
  uint8_t oledAddr;

  // ===== Motor (velocímetro) =====
  int speedFullScaleMicrosteps = 770; // visor do painel fisico (0..220km/h)
  StepperL9110::Config speedMotorCfg {}; // permite ajustar STEP_US/AMP/FLOOR/Q
};

class App {
public:
  App(TelemetryService& telemetry,
      DisplayService& ui,
      StepperL9110& speedMotor,
      SpeedGauge& speedGauge);

  void begin(const AppConfig& cfg);
  void tick();

private:
  void updateUiWifiFields();
  void applyTelemetryToUi();
  bool connectWifiWithTimeout();

  TelemetryService& _telemetry;
  DisplayService& _ui;

  // ===== Motor + Gauge =====
  StepperL9110& _speedMotor;
  SpeedGauge& _speedGauge;

  AppConfig _cfg{};
  UiStatus _st{};

  uint32_t _lastUiMs = 0;
  uint32_t _lastWifiMs = 0;

  static constexpr uint32_t UI_INTERVAL_MS   = 33;   // ~30 FPS
  static constexpr uint32_t WIFI_INTERVAL_MS = 500;  // status a cada 0,5s
};
