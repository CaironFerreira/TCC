#pragma once
#include <Arduino.h>

#include "ports/IButtonInput.h"
#include "ports/IWifiConfigPortal.h"
#include "telemetry/TelemetryService.h"
#include "ui/display/DisplayService.h"
#include "instruments/SpeedGauge.h"
#include "instruments/RpmGauge.h"
#include "instruments/FuelGauge.h"
#include "instruments/TireTempGauge.h"

struct AppConfig {
  uint16_t udpPort = 0;
  uint8_t displayLayout = 1;
};

class App {
public:
  App(IWifiConfigPortal& wifiPortal,
      IButtonInput& layoutButton,
      TelemetryService& telemetry,
      DisplayService& ui,
      SpeedGauge& speedGauge,
      RpmGauge& rpmGauge,
      FuelGauge& fuelGauge,
      TireTempGauge& tireTempGauge);

  void begin(const AppConfig& cfg);
  void tick();

private:
  void resetStatus();
  void startRuntime();
  void showWifiConnectingScreen();
  void showWifiConfigScreen();
  void updateLayoutButton(uint32_t now);
  void updateUiWifiFields();
  void updateFastUiFields();

private:
  static const uint32_t UI_INTERVAL_MS = 100;
  static const uint32_t WIFI_INTERVAL_MS = 500;
  static const uint32_t FUEL_UPDATE_INTERVAL_MS = 1000;
  static const uint32_t TEMP_UPDATE_INTERVAL_MS = 5000;

  IWifiConfigPortal& _wifiPortal;
  IButtonInput& _layoutButton;
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

  bool _runtimeStarted = false;
  bool _connectingScreenShown = false;
  bool _portalScreenShown = false;
};
