#include <Arduino.h>

#include "drivers/net/UdpReceiver.h"
#include "decoders/Forza7Decoder.h"
#include "control/TelemetryService.h"
#include "control/DisplayService.h"
#include "control/instruments/SpeedGauge.h"
#include "control/instruments/RpmGauge.h"
#include "drivers/motors/GaugeMotorTmc2208.h"
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
static const int PIN_STEP_SPEED = 25;
static const int PIN_DIR_SPEED  = 26;
static const int PIN_EN_SPEED   = 27;

// ===== Motor (RPM) =====
static const int PIN_STEP_RPM = 32;
static const int PIN_DIR_RPM  = 33;
static const int PIN_EN_RPM   = 14;

// ===== Curso real do painel =====
static const int32_t SPEED_RANGE_STEPS_0_TO_MAX = 180;
static const float   SPEED_MAX_KMH              = 220.0f;

static const int32_t RPM_RANGE_STEPS_0_TO_MAX   = 190;
static const float   RPM_MAX_VALUE              = 8000.0f;

// ===== Core =====
UdpReceiver udp;
Forza7Decoder decoder;
TelemetryService telemetry(udp, decoder);

// ===== UI =====
DisplayService ui;

// ===== Configs =====
GaugeMotorTmc2208::Config speedMotorCfg;
GaugeMotorTmc2208::Config rpmMotorCfg;

SpeedGauge::Config speedGaugeCfg;
RpmGauge::Config rpmGaugeCfg;

// ===== Objetos =====
GaugeMotorTmc2208* speedMotor = nullptr;
GaugeMotorTmc2208* rpmMotor = nullptr;

SpeedGauge* speedGauge = nullptr;
RpmGauge* rpmGauge = nullptr;

App* app = nullptr;

void setup() {
  // ===== Configuração motor do velocímetro =====
  speedMotorCfg.pinStep = PIN_STEP_SPEED;
  speedMotorCfg.pinDirection = PIN_DIR_SPEED;
  speedMotorCfg.pinEnable = PIN_EN_SPEED;
  speedMotorCfg.invertDirection = false;
  speedMotorCfg.stepPulseUs = 2;
  speedMotorCfg.stepIntervalUs = 20000;

  // ===== Configuração motor do RPM =====
  rpmMotorCfg.pinStep = PIN_STEP_RPM;
  rpmMotorCfg.pinDirection = PIN_DIR_RPM;
  rpmMotorCfg.pinEnable = PIN_EN_RPM;
  rpmMotorCfg.invertDirection = false;
  rpmMotorCfg.stepPulseUs = 2;
  rpmMotorCfg.stepIntervalUs = 3000;

  // ===== Configuração do velocímetro =====
  speedGaugeCfg.maxSpeedKmh = SPEED_MAX_KMH;
  speedGaugeCfg.minSteps = 0;
  speedGaugeCfg.maxSteps = SPEED_RANGE_STEPS_0_TO_MAX;

  // ===== Configuração do conta-giros =====
  rpmGaugeCfg.maxRpm = RPM_MAX_VALUE;
  rpmGaugeCfg.minSteps = 0;
  rpmGaugeCfg.maxSteps = RPM_RANGE_STEPS_0_TO_MAX;

  // ===== Instâncias =====
  speedMotor = new GaugeMotorTmc2208(speedMotorCfg);
  rpmMotor = new GaugeMotorTmc2208(rpmMotorCfg);

  speedGauge = new SpeedGauge(*speedMotor, speedGaugeCfg);
  rpmGauge = new RpmGauge(*rpmMotor, rpmGaugeCfg);

  app = new App(telemetry, ui, *speedMotor, *rpmMotor, *speedGauge, *rpmGauge);

  AppConfig cfg;
  cfg.wifiSsid = WIFI_SSID;
  cfg.wifiPass = WIFI_PASS;
  cfg.udpPort = UDP_PORT;
  cfg.wifiConnectTimeoutMs = WIFI_CONNECT_TIMEOUT_MS;
  cfg.oledSda = OLED_SDA;
  cfg.oledScl = OLED_SCL;
  cfg.oledAddr = OLED_ADDR;

  app->begin(cfg);
}

void loop() {
  if (app) {
    app->tick();
  }
}