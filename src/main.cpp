#include <Arduino.h>

#include "drivers/net/UdpReceiver.h"
#include "decoders/Forza7Decoder.h"
#include "control/TelemetryService.h"
#include "control/DisplayService.h"
#include "control/instruments/SpeedGauge.h"
#include "drivers/motors/GaugeMotorL9110Pwm.h"
#include "app/App.h"

// ===================== CONFIG =====================
static const char* WIFI_SSID = "CAIRON";
static const char* WIFI_PASS = "12345678";

static const uint16_t UDP_PORT = 5300;
static const uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;

static const int OLED_SDA = 21;
static const int OLED_SCL = 22;
static const uint8_t OLED_ADDR = 0x3C;

// ===== Motor (Velocímetro) =====
static const int PIN_A1 = 25;
static const int PIN_A2 = 26;
static const int PIN_B1 = 27;
static const int PIN_B2 = 14;

// Curso real do painel (índice calibrado)
static const int   SPEED_RANGE_STEPS_0_TO_MAX = 3450;
static const float SPEED_MAX_KMH              = 220.0f;


// Tamanho da tabela elétrica seno/cosseno
static const int MICROSTEPS_TABLE_360 = 600;
// ================================================

// ===== Core =====
UdpReceiver udp;
Forza7Decoder decoder;
TelemetryService telemetry(udp, decoder);

// ===== UI =====
DisplayService ui;

// ===== Motor PWM (L9110) =====
GaugeMotorL9110Pwm::Config motorCfg;
GaugeMotorL9110Pwm speedMotor(motorCfg);

// ===== Gauge =====
SpeedGauge::Config gaugeCfg;
SpeedGauge speedGauge(speedMotor, gaugeCfg);

// ===== App =====
App app(telemetry, ui, speedMotor, speedGauge);

void setup() {
  // ===== Motor config =====
  motorCfg.pinA1 = PIN_A1;
  motorCfg.pinA2 = PIN_A2;
  motorCfg.pinB1 = PIN_B1;
  motorCfg.pinB2 = PIN_B2;

  motorCfg.pwmFreq = 20000;
  motorCfg.pwmResBits = 8;

  motorCfg.microsteps360 = MICROSTEPS_TABLE_360; // mantenha 600 por enquanto

  motorCfg.ampMax = 200;     // um pouco abaixo do seu 215
  motorCfg.ampFloor = 35;   // torque mínimo conservador (ajuste depois)

  motorCfg.invertDirection = true;

  // ZERE (ou neutre) “softeners” inicialmente, se existirem:
  motorCfg.deadMag = 0.0f;
  motorCfg.softFloorMag = 0.0f;

  // Tabela elétrica (fase)
  motorCfg.microsteps360 = MICROSTEPS_TABLE_360; // 600

  motorCfg.ampMax = 215;
  motorCfg.ampFloor = 28;

  motorCfg.deadMag = 0.015f;
  motorCfg.softFloorMag = 0.2f;

  // Ajuste de sentido (mantém o seu comportamento atual)
  motorCfg.invertDirection = true;

// ===== Gauge config (BASELINE) =====
gaugeCfg.speedMaxKmh = SPEED_MAX_KMH;
gaugeCfg.rangeSteps0ToMax = SPEED_RANGE_STEPS_0_TO_MAX;

// Comece “lento e suave” e depois aumente
gaugeCfg.maxStepsPerS = 2500.0f;
gaugeCfg.maxAccelStepsPerS2 = 8000.0f;

// Homing
gaugeCfg.homeDirSign = -1;
gaugeCfg.homeStepInc = 6;
gaugeCfg.homeStepMs  = 2;
gaugeCfg.homeTimeMs  = 1200;

// (Se você implementar kpVel no Config)
gaugeCfg.kpVel = 25.0f;

  // ===== AppConfig =====
  AppConfig cfg;

  cfg.wifiSsid = WIFI_SSID;
  cfg.wifiPass = WIFI_PASS;
  cfg.udpPort = UDP_PORT;
  cfg.wifiConnectTimeoutMs = WIFI_CONNECT_TIMEOUT_MS;

  cfg.oledSda = OLED_SDA;
  cfg.oledScl = OLED_SCL;
  cfg.oledAddr = OLED_ADDR;

  // Velocímetro
  cfg.speedMaxKmh = SPEED_MAX_KMH;
  cfg.speedRangeSteps0ToMax = SPEED_RANGE_STEPS_0_TO_MAX;

  // Copia configs para o App
  cfg.speedMotorCfg = motorCfg;
  cfg.speedGaugeCfg = gaugeCfg;

  app.begin(cfg);
}

void loop() {
  app.tick();
}