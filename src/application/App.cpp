#include "application/App.h"
App::App(IWifiConfigPortal& wifiPortal,
         IButtonInput& layoutButton,
         IClock& clock,
         ITelemetrySource& telemetry,
         IStatusDisplay& ui,
         IInstrumentCluster& instruments)
  : _wifiPortal(wifiPortal),
    _layoutButton(layoutButton),
    _clock(clock),
    _telemetry(telemetry),
    _ui(ui),
    _instruments(instruments) {}

void App::begin(const AppConfig& cfg) {
  _cfg = cfg;

  resetStatus();
  _layoutButton.begin();

  _runtimeStarted = false;
  _connectingScreenShown = false;
  _portalScreenShown = false;

  _wifiPortal.begin();

  if (_wifiPortal.isPortalActive()) {
    showWifiConfigScreen();
  } else if (_wifiPortal.isConnected()) {
    startRuntime();
  } else {
    showWifiConnectingScreen();
  }
}

void App::resetStatus() {
  _st.setSsid("");
  _st.setIp("-");
  _st.wifiConnected = false;
  _st.telemetryValid = false;
  _st.telemetrySignalPresent = false;
  _st.telemetryInputStatus = TelemetryInputStatus::Idle;
  _st.telemetryDiscardedPackets = 0;
  _st.telemetryReceiveErrors = 0;
  _st.speedKmh = 0.0f;
  _st.gear = 0;
  _st.rpm = 0;
  _st.lapNumber = 0;
  _st.racePosition = 0;
  _st.fuel = 0.0f;
  _st.tireTempAvg = 0.0f;
  _st.tireTempFL = 0.0f;
  _st.tireTempFR = 0.0f;
  _st.tireTempRL = 0.0f;
  _st.tireTempRR = 0.0f;

  _lastUiMs = 0;
  _lastWifiMs = 0;
  _lastTempUpdateMs = 0;
}

void App::showWifiConfigScreen() {
  _ui.begin(_cfg.displayLayout);
  _ui.showMessage("Wi-Fi SimHub",
                  "Conecte no Wi-Fi SimHub",
                  "Abra http://simhub",
                  "Informe rede e senha");

  _portalScreenShown = true;
}

void App::showWifiConnectingScreen() {
  _ui.begin(_cfg.displayLayout);
  _ui.showMessage("Conectando Wi-Fi",
                  "Usando rede salva",
                  "Aguarde alguns segundos",
                  "Portal abre se falhar");

  _connectingScreenShown = true;
}

void App::startRuntime() {
  resetStatus();

  _ui.begin(_cfg.displayLayout);

  _instruments.begin();
  _telemetry.begin(_cfg.udpPort);

  const uint32_t now = _clock.nowMs();
  _lastTempUpdateMs = now - TEMP_UPDATE_INTERVAL_MS;

  updateUiWifiFields();
  updateFastUiFields();

  _ui.setStatus(_st);
  _ui.tick();

  _runtimeStarted = true;
  _connectingScreenShown = false;
  _portalScreenShown = false;
}

void App::updateUiWifiFields() {
  _st.wifiConnected = _wifiPortal.isConnected();

  if (_st.wifiConnected) {
    _wifiPortal.copyConnectedSsid(_st.ssid, sizeof(_st.ssid));
    _wifiPortal.copyLocalIp(_st.ip, sizeof(_st.ip));
  } else {
    _st.setSsid("Nao conectado");
    _st.setIp("-");
  }
}

void App::updateFastUiFields() {
  _st.telemetryValid = _telemetry.hasValidTelemetry();
  _st.telemetrySignalPresent = _telemetry.hasAnySignal();
  _st.telemetryInputStatus = _telemetry.inputStatus();
  _st.telemetryDiscardedPackets = _telemetry.discardedPacketCount();
  _st.telemetryReceiveErrors = _telemetry.receiveErrorCount();
  _st.speedKmh = _telemetry.speedKmh();
  _st.rpm = static_cast<int>(_telemetry.rpm());
  _st.gear = _telemetry.gear();
  _st.lapNumber = _telemetry.lapNumber();
  _st.racePosition = _telemetry.racePosition();
}

void App::tick() {
  _wifiPortal.tick();

  if (_wifiPortal.isPortalActive()) {
    if (!_portalScreenShown) {
      showWifiConfigScreen();
    }

    return;
  }

  if (!_runtimeStarted && _wifiPortal.isConnected()) {
    startRuntime();
  }

  if (!_runtimeStarted) {
    if (!_connectingScreenShown) {
      showWifiConnectingScreen();
    }

    return;
  }

  const uint32_t now = _clock.nowMs();
  const uint32_t nowMicros = _clock.nowMicros();

  updateLayoutButton(now);

  _telemetry.tick();

  // gauges rápidos
  _instruments.setSpeedKmh(_telemetry.speedKmh());
  _instruments.setRpm(_telemetry.rpm());

  if (_telemetry.hasValidTelemetry()) {
    const float fuel = _telemetry.fuelLevel();
    _instruments.setFuelLevel(fuel);
    _st.fuel = fuel;
  }

  // temp (5s)
  if (now - _lastTempUpdateMs >= TEMP_UPDATE_INTERVAL_MS) {
    if (_telemetry.hasValidTelemetry()) {
      _lastTempUpdateMs = now;

      float temp = _telemetry.tireTempAvgC();
      _instruments.setTireTemperature(temp);
      _st.tireTempAvg = temp;
      _st.tireTempFL = _telemetry.tireTempFLC();
      _st.tireTempFR = _telemetry.tireTempFRC();
      _st.tireTempRL = _telemetry.tireTempRLC();
      _st.tireTempRR = _telemetry.tireTempRRC();
    }
  }

  _instruments.tick(nowMicros);

  // wifi
  if (now - _lastWifiMs >= WIFI_INTERVAL_MS) {
    _lastWifiMs = now;
    updateUiWifiFields();
  }

  // UI
  if (now - _lastUiMs >= UI_INTERVAL_MS) {
    _lastUiMs = now;

    updateFastUiFields();
    _ui.setStatus(_st);
    _ui.tick();
  }
}

void App::updateLayoutButton(uint32_t now) {
  _layoutButton.tick(now);

  if (_layoutButton.wasPressed()) {
    updateFastUiFields();
    _ui.setStatus(_st);

    if (_ui.nextLayout()) {
      _ui.tick();
    }
  }
}
