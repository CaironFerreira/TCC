#pragma once
#include <Arduino.h>

#include "../control/TelemetryService.h"
#include "../control/DisplayService.h"
#include "../control/instruments/SpeedGauge.h"
#include "../control/instruments/RpmGauge.h"
#include "../control/instruments/FuelGauge.h"
#include "../control/instruments/TireTempGauge.h"

struct AppConfig {
  const char* wifiSsid = nullptr;
  const char* wifiPass = nullptr;
  uint16_t udpPort = 0;
  uint32_t wifiConnectTimeoutMs = 15000;
  uint8_t displayLayout = 1;
};

class App {
public:
  App(TelemetryService& telemetry,
      DisplayService& ui,
      SpeedGauge& speedGauge,
      RpmGauge& rpmGauge,
      FuelGauge& fuelGauge,
      TireTempGauge& tireTempGauge);

  void begin(const AppConfig& cfg);
  void tick();

private:
  void updateUiWifiFields();
  void updateFastUiFields();

private:
  static const uint32_t UI_INTERVAL_MS = 100;
  static const uint32_t WIFI_INTERVAL_MS = 500;
  static const uint32_t FUEL_UPDATE_INTERVAL_MS = 15000;
  static const uint32_t TEMP_UPDATE_INTERVAL_MS = 5000;

  TelemetryService& _telemetry;
  DisplayService& _ui;
  SpeedGauge& _speedGauge;
  RpmGauge& _rpmGauge;
  FuelGauge& _fuelGauge;
  TireTempGauge& _tireTempGauge;

  AppConfig _cfg{};
  UiStatus _st{};

  uint32_t _lastUiMs = 0;
  uint32_t _lastWifiMs = 0;
  uint32_t _lastFuelUpdateMs = 0;
  uint32_t _lastTempUpdateMs = 0;
};