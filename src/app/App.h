#pragma once
#include <Arduino.h>

#include "control/TelemetryService.h"
#include "control/DisplayService.h"
#include "drivers/motors/GaugeMotorL9110Pwm.h"
#include "control/instruments/SpeedGauge.h"

/*
  ===== Flags de teste (compile-time) =====

  TESTE 1 (timing):
    - Defina APP_TEST_DISABLE_UI para desabilitar chamadas de UI (OLED)
    - Defina APP_TEST_DISABLE_WIFI para desabilitar conexão/status Wi-Fi
    - Defina APP_TEST_DISABLE_TELEMETRY para desabilitar tick/frames de telemetria
  Exemplo no platformio.ini (build_flags):
    -DAPP_TEST_DISABLE_UI
    -DAPP_TEST_DISABLE_WIFI
    -DAPP_TEST_DISABLE_TELEMETRY

  TESTE 2 (torque):
    - Defina APP_TEST_FORCE_AMP_FLOOR e opcionalmente APP_TEST_AMP_FLOOR_VALUE
  Exemplo:
    -DAPP_TEST_FORCE_AMP_FLOOR
    -DAPP_TEST_AMP_FLOOR_VALUE=20
*/

#ifndef APP_TEST_AMP_FLOOR_VALUE
#define APP_TEST_AMP_FLOOR_VALUE 20
#endif

struct AppConfig {
  const char* wifiSsid;
  const char* wifiPass;
  uint16_t udpPort;
  uint32_t wifiConnectTimeoutMs;

  int oledSda;
  int oledScl;
  uint8_t oledAddr;

  // ===== Velocímetro =====
  float speedMaxKmh = 240.0f;        // escala do painel/jogo (0..speedMaxKmh)
  int speedRangeSteps0ToMax = 3450;  // calibrado: 0..fim do painel

  // Configs
  GaugeMotorL9110Pwm::Config speedMotorCfg{}; // PWM, pinos, amp, dead/soft, etc.
  SpeedGauge::Config speedGaugeCfg{};         // limites de velocidade/aceleração
};

class App {
public:
  App(TelemetryService& telemetry,
      DisplayService& ui,
      GaugeMotorL9110Pwm& speedMotor,
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
  GaugeMotorL9110Pwm& _speedMotor;
  SpeedGauge& _speedGauge;

  AppConfig _cfg{};
  UiStatus _st{};

  uint32_t _lastUiMs = 0;
  uint32_t _lastWifiMs = 0;
  uint32_t _lastGaugeMs = 0;

  // Intervalos (mantive os seus)
  static constexpr uint32_t UI_INTERVAL_MS    = 33;  // ~30 FPS
  static constexpr uint32_t WIFI_INTERVAL_MS  = 500; // status a cada 0,5s
  static constexpr uint32_t GAUGE_INTERVAL_MS = 2;   // update do ponteiro

  // Helpers para ler se o build está em modo teste
  static constexpr bool kDisableUi =
  #ifdef APP_TEST_DISABLE_UI
    true;
  #else
    false;
  #endif

  static constexpr bool kDisableWifi =
  #ifdef APP_TEST_DISABLE_WIFI
    true;
  #else
    false;
  #endif

  static constexpr bool kDisableTelemetry =
  #ifdef APP_TEST_DISABLE_TELEMETRY
    true;
  #else
    false;
  #endif

  static constexpr bool kForceAmpFloor =
  #ifdef APP_TEST_FORCE_AMP_FLOOR
    true;
  #else
    false;
  #endif
  ;
};
