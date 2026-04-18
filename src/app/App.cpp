#include "app/App.h"

App::App(IWifiConfigPortal& wifiPortal,
         IButtonInput& layoutButton,
         TelemetryService& telemetry,
         DisplayService& ui,
         SpeedGauge& speedGauge,
         RpmGauge& rpmGauge,
         FuelGauge& fuelGauge,
         TireTempGauge& tireTempGauge)
  : _wifiPortal(wifiPortal),
    _layoutButton(layoutButton),
    _telemetry(telemetry),
    _ui(ui),
    _speedGauge(speedGauge),
    _rpmGauge(rpmGauge),
    _fuelGauge(fuelGauge),
    _tireTempGauge(tireTempGauge) {}

void App::begin(const AppConfig& cfg) {
  _cfg = cfg;

  resetStatus();
  _layoutButton.begin();

  _runtimeStarted = false;
  _connectingScreenShown = false;
  _portalScreenShown = false;

  if (_wifiPortal.begin()) {
    startRuntime();
  } else if (_wifiPortal.isPortalActive()) {
    showWifiConfigScreen();
  } else {
    showWifiConnectingScreen();
  }
}

void App::resetStatus() {
  _st.ssid = "";
  _st.ip = "-";
  _st.wifiConnected = false;
  _st.speedKmh = 0.0f;
  _st.gear = 0;
  _st.rpm = 0;
  _st.fuel = 0.0f;
  _st.tireTempAvg = 0.0f;

  _lastUiMs = 0;
  _lastWifiMs = 0;
  _lastFuelUpdateMs = 0;
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

  _speedGauge.begin();
  _rpmGauge.begin();
  _fuelGauge.begin();
  _tireTempGauge.begin();

  _telemetry.begin(_cfg.udpPort);

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
    _st.ssid = _wifiPortal.connectedSsid();
    _st.ip = _wifiPortal.localIp();
  } else {
    _st.ssid = "Nao conectado";
    _st.ip = "-";
  }
}

void App::updateFastUiFields() {
  _st.speedKmh = _telemetry.speedKmh();
  _st.rpm = static_cast<int>(_telemetry.rpm());
  _st.gear = _telemetry.gear();
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

  const uint32_t now = millis();
  const uint32_t nowMicros = micros();

  updateLayoutButton(now);

  _telemetry.tick();

  // gauges rápidos
  _speedGauge.setSpeedKmh(_telemetry.speedKmh());
  _rpmGauge.setRpm(_telemetry.rpm());

  // fuel (15s)
  if (now - _lastFuelUpdateMs >= FUEL_UPDATE_INTERVAL_MS) {
    _lastFuelUpdateMs = now;

    float fuel = _telemetry.fuelLevel();
    _fuelGauge.setFuelLevel(fuel);
    _st.fuel = fuel;
  }

  // temp (5s)
  if (now - _lastTempUpdateMs >= TEMP_UPDATE_INTERVAL_MS) {
    _lastTempUpdateMs = now;

    float temp = _telemetry.tireTempAvgC();
    _tireTempGauge.setTemperature(temp);
    _st.tireTempAvg = temp;
  }

  _speedGauge.tick(nowMicros);
  _rpmGauge.tick(nowMicros);
  _fuelGauge.tick(nowMicros);
  _tireTempGauge.tick(nowMicros);

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
