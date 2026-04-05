#include <Arduino.h>

#include "drivers/net/UdpReceiver.h"
#include "decoders/Forza7Decoder.h"
#include "control/TelemetryService.h"
#include "control/DisplayService.h"
#include "control/instruments/SpeedGauge.h"
#include "control/instruments/RpmGauge.h"
#include "control/instruments/FuelGauge.h"
#include "control/instruments/TireTempGauge.h"
#include "drivers/motors/GaugeMotorTmc2208.h"
#include "app/App.h"

// ===================== CONFIG =====================
static const char* WIFI_SSID = "CAIRON";
static const char* WIFI_PASS = "12345678";

static const uint16_t UDP_PORT = 5300;
static const uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;

// ===== Motor (Velocímetro) =====
static const int PIN_STEP_SPEED = 25;
static const int PIN_DIR_SPEED  = 26;
static const int PIN_EN_SPEED   = 27;

// ===== Motor (RPM) =====
static const int PIN_STEP_RPM = 12;
static const int PIN_DIR_RPM  = 13;
static const int PIN_EN_RPM   = 14;

// ===== Motor (Combustível) =====
static const int PIN_STEP_FUEL = 15;
static const int PIN_DIR_FUEL  = 19;
static const int PIN_EN_FUEL   = 22;

// ===== Motor (Temperatura pneus) =====
static const int PIN_STEP_TEMP = 32;
static const int PIN_DIR_TEMP  = 33;
static const int PIN_EN_TEMP   = 17;

// ===== Curso real do painel =====
static const int32_t SPEED_RANGE_STEPS_0_TO_MAX = 180;
static const float   SPEED_MAX_KMH              = 220.0f;

static const int32_t RPM_RANGE_STEPS_0_TO_MAX   = 190;
static const float   RPM_MAX_VALUE              = 8000.0f;

static const int32_t FUEL_RANGE_STEPS_0_TO_MAX  = 180;
static const int32_t TEMP_RANGE_STEPS_0_TO_MAX  = 180;

// ===== Core =====
UdpReceiver udp;
Forza7Decoder decoder;
TelemetryService telemetry(udp, decoder);

// ===== UI =====
DisplayService ui;

// ===== Configs dos motores =====
GaugeMotorTmc2208::Config speedMotorCfg;
GaugeMotorTmc2208::Config rpmMotorCfg;
GaugeMotorTmc2208::Config fuelMotorCfg;
GaugeMotorTmc2208::Config tempMotorCfg;

// ===== Configs dos instrumentos =====
SpeedGauge::Config speedGaugeCfg;
RpmGauge::Config rpmGaugeCfg;
FuelGauge::Config fuelGaugeCfg;
TireTempGauge::Config tireTempGaugeCfg;

// ===== Objetos =====
GaugeMotorTmc2208* speedMotor = nullptr;
GaugeMotorTmc2208* rpmMotor   = nullptr;
GaugeMotorTmc2208* fuelMotor  = nullptr;
GaugeMotorTmc2208* tempMotor  = nullptr;

SpeedGauge* speedGauge       = nullptr;
RpmGauge* rpmGauge           = nullptr;
FuelGauge* fuelGauge         = nullptr;
TireTempGauge* tireTempGauge = nullptr;

App* app = nullptr;

void setup() {
  // =========================================================
  // Configuração dos motores físicos
  // =========================================================

  // ===== Motor do velocímetro =====
  speedMotorCfg.pinStep = PIN_STEP_SPEED;
  speedMotorCfg.pinDirection = PIN_DIR_SPEED;
  speedMotorCfg.pinEnable = PIN_EN_SPEED;
  speedMotorCfg.invertDirection = false;
  speedMotorCfg.stepPulseUs = 2;

  // ===== Motor do RPM =====
  rpmMotorCfg.pinStep = PIN_STEP_RPM;
  rpmMotorCfg.pinDirection = PIN_DIR_RPM;
  rpmMotorCfg.pinEnable = PIN_EN_RPM;
  rpmMotorCfg.invertDirection = false;
  rpmMotorCfg.stepPulseUs = 2;

  // ===== Motor do combustível =====
  fuelMotorCfg.pinStep = PIN_STEP_FUEL;
  fuelMotorCfg.pinDirection = PIN_DIR_FUEL;
  fuelMotorCfg.pinEnable = PIN_EN_FUEL;
  fuelMotorCfg.invertDirection = false;
  fuelMotorCfg.stepPulseUs = 2;

  // ===== Motor da temperatura =====
  tempMotorCfg.pinStep = PIN_STEP_TEMP;
  tempMotorCfg.pinDirection = PIN_DIR_TEMP;
  tempMotorCfg.pinEnable = PIN_EN_TEMP;
  tempMotorCfg.invertDirection = false;
  tempMotorCfg.stepPulseUs = 2;

  // =========================================================
  // Configuração lógica dos instrumentos
  // =========================================================

  // ===== Velocímetro =====
  speedGaugeCfg.maxSpeedKmh = SPEED_MAX_KMH;
  speedGaugeCfg.minSteps = 0;
  speedGaugeCfg.maxSteps = SPEED_RANGE_STEPS_0_TO_MAX;
  speedGaugeCfg.normalStepsPerSec = 50.0f;
  speedGaugeCfg.fastStepsPerSec = 500.0f;

  // ===== Conta-giros =====
  rpmGaugeCfg.maxRpm = RPM_MAX_VALUE;
  rpmGaugeCfg.minSteps = 0;
  rpmGaugeCfg.maxSteps = RPM_RANGE_STEPS_0_TO_MAX;
  rpmGaugeCfg.normalStepsPerSec = 60.0f;
  rpmGaugeCfg.fastStepsPerSec = 300.0f;

  // ===== Combustível =====
  fuelGaugeCfg.minFuelLevel = 0.0f;
  fuelGaugeCfg.maxFuelLevel = 1.0f;
  fuelGaugeCfg.minSteps = 0;
  fuelGaugeCfg.maxSteps = FUEL_RANGE_STEPS_0_TO_MAX;
  fuelGaugeCfg.riseStepsPerSec = 120.0f;
  fuelGaugeCfg.fallStepsPerSec = 120.0f;

  // ===== Temperatura média dos pneus =====
  tireTempGaugeCfg.minTemp = 20.0f;
  tireTempGaugeCfg.maxTemp = 120.0f;
  tireTempGaugeCfg.minSteps = 0;
  tireTempGaugeCfg.maxSteps = TEMP_RANGE_STEPS_0_TO_MAX;
  tireTempGaugeCfg.riseStepsPerSec = 180.0f;
  tireTempGaugeCfg.fallStepsPerSec = 220.0f;

  // =========================================================
  // Instâncias dos motores
  // =========================================================
  speedMotor = new GaugeMotorTmc2208(speedMotorCfg);
  rpmMotor   = new GaugeMotorTmc2208(rpmMotorCfg);
  fuelMotor  = new GaugeMotorTmc2208(fuelMotorCfg);
  tempMotor  = new GaugeMotorTmc2208(tempMotorCfg);

  // =========================================================
  // Instâncias dos instrumentos
  // =========================================================
  speedGauge    = new SpeedGauge(*speedMotor, speedGaugeCfg);
  rpmGauge      = new RpmGauge(*rpmMotor, rpmGaugeCfg);
  fuelGauge     = new FuelGauge(*fuelMotor, fuelGaugeCfg);
  tireTempGauge = new TireTempGauge(*tempMotor, tireTempGaugeCfg);

  // =========================================================
  // App principal
  // =========================================================
  app = new App(
    telemetry,
    ui,
    *speedMotor,
    *rpmMotor,
    *fuelMotor,
    *tempMotor,
    *speedGauge,
    *rpmGauge,
    *fuelGauge,
    *tireTempGauge
  );

  AppConfig cfg;
  cfg.wifiSsid = WIFI_SSID;
  cfg.wifiPass = WIFI_PASS;
  cfg.udpPort = UDP_PORT;
  cfg.wifiConnectTimeoutMs = WIFI_CONNECT_TIMEOUT_MS;

  app->begin(cfg);
}

void loop() {
  if (app) {
    app->tick();
  }
}