// src/main.cpp
#include <Arduino.h>

#include "drivers/net/UdpReceiver.h"
#include "decoders/Forza7Decoder.h"
#include "control/TelemetryService.h"
#include "control/DisplayService.h"
#include "control/instruments/SpeedGauge.h"
#include "drivers/motors/StepperL9110.h"
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

static const int CH_A1 = 0;
static const int CH_A2 = 1;
static const int CH_B1 = 2;
static const int CH_B2 = 3;

// Curso real do painel (0–220 km/h)
static const int SPEED_FULLSCALE_STEPS = 770;
// ================================================

// ===== Core =====
UdpReceiver udp;
Forza7Decoder decoder;
TelemetryService telemetry(udp, decoder);

// ===== UI =====
DisplayService ui;

// ===== Motor + Gauge =====
StepperL9110::Pins speedPins {
  PIN_A1, PIN_A2,
  PIN_B1, PIN_B2
};

StepperL9110 speedMotor(
  speedPins,
  CH_A1, CH_A2,
  CH_B1, CH_B2
);

SpeedGauge speedGauge(speedMotor, SPEED_FULLSCALE_STEPS);

// ===== App =====
App app(telemetry, ui, speedMotor, speedGauge);

void setup() {
  AppConfig cfg;

  cfg.wifiSsid = WIFI_SSID;
  cfg.wifiPass = WIFI_PASS;
  cfg.udpPort = UDP_PORT;
  cfg.wifiConnectTimeoutMs = WIFI_CONNECT_TIMEOUT_MS;

  cfg.oledSda = OLED_SDA;
  cfg.oledScl = OLED_SCL;
  cfg.oledAddr = OLED_ADDR;

  cfg.speedFullScaleMicrosteps = SPEED_FULLSCALE_STEPS;

  // Configuração elétrica/mecânica do motor
  cfg.speedMotorCfg.stepUs = 2000;
  cfg.speedMotorCfg.amp    = 200;
  cfg.speedMotorCfg.floor  = 18;
  cfg.speedMotorCfg.q      = 32;

  cfg.speedMotorCfg.invertDir = true;
  app.begin(cfg);
}

void loop() {
  app.tick();
}
