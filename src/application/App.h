#pragma once
#include <stdint.h>

#include "ports/IButtonInput.h"
#include "ports/IClock.h"
#include "ports/IInstrumentCluster.h"
#include "ports/IStatusDisplay.h"
#include "ports/ITelemetrySource.h"
#include "ports/IWifiConfigPortal.h"
#include "domain/UiStatus.h"

struct AppConfig {
  uint16_t udpPort = 0;
  uint8_t displayLayout = 1;
};

class App {
public:
  App(IWifiConfigPortal& wifiPortal,
      IButtonInput& layoutButton,
      IClock& clock,
      ITelemetrySource& telemetry,
      IStatusDisplay& ui,
      IInstrumentCluster& instruments);

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
  static const uint32_t TEMP_UPDATE_INTERVAL_MS = 5000;

  IWifiConfigPortal& _wifiPortal;
  IButtonInput& _layoutButton;
  IClock& _clock;
  ITelemetrySource& _telemetry;
  IStatusDisplay& _ui;
  IInstrumentCluster& _instruments;

  AppConfig _cfg{};
  UiStatus _st{};

  uint32_t _lastUiMs = 0;
  uint32_t _lastWifiMs = 0;
  uint32_t _lastTempUpdateMs = 0;

  bool _runtimeStarted = false;
  bool _connectingScreenShown = false;
  bool _portalScreenShown = false;
};
