#include <Arduino.h>

#include "drivers/net/UdpReceiver.h"
#include "decoders/Forza7Decoder.h"
#include "control/TelemetryService.h"
#include "control/DisplayService.h"
#include "control/instruments/SpeedGauge.h"
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
static const int PIN_STEP = 25;
static const int PIN_DIR  = 26;
static const int PIN_EN   = 27;

// Curso real do painel
static const int32_t SPEED_RANGE_STEPS_0_TO_MAX = 180;
static const float   SPEED_MAX_KMH              = 220.0f;

// ===== Core =====
UdpReceiver udp;
Forza7Decoder decoder;
TelemetryService telemetry(udp, decoder);

// ===== UI =====
DisplayService ui;

// ===== Configs =====
GaugeMotorTmc2208::Config motorCfg;
SpeedGauge::Config speedGaugeCfg;

// ===== Objetos =====
GaugeMotorTmc2208* speedMotor = nullptr;
SpeedGauge* speedGauge = nullptr;
App* app = nullptr;

void setup() {
  motorCfg.pinStep = PIN_STEP;
  motorCfg.pinDirection = PIN_DIR;
  motorCfg.pinEnable = PIN_EN;
  motorCfg.invertDirection = false;
  motorCfg.stepPulseUs = 2;
  motorCfg.stepIntervalUs = 1200;

  speedGaugeCfg.maxSpeedKmh = SPEED_MAX_KMH;
  speedGaugeCfg.minSteps = 0;
  speedGaugeCfg.maxSteps = SPEED_RANGE_STEPS_0_TO_MAX;

  speedMotor = new GaugeMotorTmc2208(motorCfg);
  speedGauge = new SpeedGauge(*speedMotor, speedGaugeCfg);
  app = new App(telemetry, ui, *speedMotor, *speedGauge);

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