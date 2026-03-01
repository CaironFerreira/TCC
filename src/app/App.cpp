#include "App.h"
#include <WiFi.h>
#include <math.h>   // isfinite

App::App(TelemetryService& telemetry,
         DisplayService& ui,
         GaugeMotorL9110Pwm& speedMotor,
         SpeedGauge& speedGauge)
  : _telemetry(telemetry),
    _ui(ui),
    _speedMotor(speedMotor),
    _speedGauge(speedGauge) {}

void App::begin(const AppConfig& cfg) {
  _cfg = cfg;

  // ===== TESTE 2: força ampFloor (torque mínimo) se habilitado =====
  if (kForceAmpFloor) {
    _cfg.speedMotorCfg.ampFloor = APP_TEST_AMP_FLOOR_VALUE;
  }

  // OLED
  if (!kDisableUi) {
    _ui.begin(_cfg.oledSda, _cfg.oledScl, _cfg.oledAddr);
  }

  // estado inicial
  _st.ssid = _cfg.wifiSsid;
  _st.ip = "-";
  _st.wifiConnected = false;
  _st.speedKmh = 0.0f;
  _st.rpm = 0;
  _st.gear = 0;

  if (!kDisableUi) {
    _ui.setStatus(_st);
    _ui.tick();
  }

  // ===== Motor + Gauge (velocímetro) =====
  _speedMotor.begin(_cfg.speedMotorCfg);

  // aplica configs (garante range e escala do painel)
  auto gcfg = _cfg.speedGaugeCfg;
  gcfg.speedMaxKmh = _cfg.speedMaxKmh;
  gcfg.rangeSteps0ToMax = _cfg.speedRangeSteps0ToMax;

  _speedGauge.begin(gcfg);
  _speedGauge.homeToStop();

  // Wi-Fi (conexão inicial com timeout)
  if (!kDisableWifi) {
    connectWifiWithTimeout();
    updateUiWifiFields();

    if (!kDisableUi) {
      _ui.setStatus(_st);
      _ui.tick();
    }
  }

  // UDP / Telemetria
  if (!kDisableTelemetry) {
    _telemetry.begin(_cfg.udpPort);
  }

  _lastUiMs = 0;
  _lastWifiMs = 0;
  _lastGaugeMs = 0;
}

bool App::connectWifiWithTimeout() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(_cfg.wifiSsid, _cfg.wifiPass);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(150);

    // Durante a conexão, atualiza tela somente se UI estiver habilitada
    if (!kDisableUi) {
      updateUiWifiFields();
      _ui.setStatus(_st);
      _ui.tick();
    }

    if (millis() - start > _cfg.wifiConnectTimeoutMs) {
      if (!kDisableUi) {
        updateUiWifiFields();
        _ui.setStatus(_st);
        _ui.tick();
      }
      return false;
    }
  }

  return true;
}

void App::updateUiWifiFields() {
  wl_status_t wst = WiFi.status();
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
    _st.speedKmh = f.speedKmh;
    _st.rpm = f.rpm;
    _st.gear = f.gear;
  } else {
    _st.speedKmh = 0.0f;
    _st.rpm = 0;
    _st.gear = 0;
  }
}

void App::tick() {
  // ===== Telemetria =====
  if (!kDisableTelemetry) {
    _telemetry.tick();
  }

  // ===== Telemetria -> velocidade (único ponto de decisão) =====
  float speedKmh = 0.0f;
  if (!kDisableTelemetry) {
    const auto& f = _telemetry.lastFrame();
    if (f.valid) speedKmh = f.speedKmh;
  }

  // Sanidade (evita NaN, negativos e extrapolações)
  if (!isfinite(speedKmh) || speedKmh < 0.0f) speedKmh = 0.0f;
  if (speedKmh > _cfg.speedMaxKmh) speedKmh = _cfg.speedMaxKmh;

  // ===== Atualiza alvo do gauge =====
  _speedGauge.setSpeedKmh(speedKmh);

  // ===== Atualiza o ponteiro =====
#ifdef APP_TEST_GAUGE_ALWAYS_UPDATE
  _speedGauge.update();
  const uint32_t now = millis();
#else
  const uint32_t now = millis();
  if (now - _lastGaugeMs >= GAUGE_INTERVAL_MS) {
    _lastGaugeMs = now;
    _speedGauge.update();
  }
#endif

  // ===== Wi-Fi =====
  if (!kDisableWifi) {
    if (now - _lastWifiMs >= WIFI_INTERVAL_MS) {
      _lastWifiMs = now;
      updateUiWifiFields();
    }
  }

  // ===== UI =====
  if (kDisableUi) return;

  if (now - _lastUiMs < UI_INTERVAL_MS) return;
  _lastUiMs = now;

  if (!kDisableTelemetry) {
    applyTelemetryToUi();
  } else {
    _st.speedKmh = 0.0f;
    _st.rpm = 0;
    _st.gear = 0;
  }

  _ui.setStatus(_st);
  _ui.tick();
}