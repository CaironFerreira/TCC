#include <Arduino.h>

#include "config/BoardConfig.h"
#include "drivers/network/wifi/WiFiConfigPortal.h"

#include "drivers/network/udp/UdpReceiver.h"
#include "telemetry/decoders/Forza7Decoder.h"
#include "telemetry/TelemetryService.h"
#include "ui/display/DisplayService.h"
#include "instruments/SpeedGauge.h"
#include "instruments/RpmGauge.h"
#include "instruments/FuelGauge.h"
#include "instruments/TireTempGauge.h"
#include "drivers/motors/GaugeMotorTmc2208.h"
#include "app/App.h"

// ===== Portal Wi-Fi =====
static WiFiConfigPortal::Config makePortalConfig() {
  WiFiConfigPortal::Config cfg;
  cfg.apSsid = "ESP32-Config";
  cfg.apPassword = "12345678";
  cfg.connectTimeoutMs = WIFI_CONNECT_TIMEOUT_MS;
  return cfg;
}

static WiFiConfigPortal wifiPortal(makePortalConfig());
static bool appStarted = false;

// ===== Core =====
static UdpReceiver udp;
static Forza7Decoder decoder;
static TelemetryService telemetry(udp, decoder);

// ===== UI =====
static DisplayService ui;

static GaugeMotorTmc2208::Config makeSpeedMotorConfig() {
  GaugeMotorTmc2208::Config cfg;
  cfg.pinStep = PIN_STEP_SPEED;
  cfg.pinDirection = PIN_DIR_SPEED;
  cfg.pinEnable = PIN_EN_SPEED;
  cfg.invertDirection = false;
  cfg.stepPulseUs = 2;
  return cfg;
}

static GaugeMotorTmc2208::Config makeRpmMotorConfig() {
  GaugeMotorTmc2208::Config cfg;
  cfg.pinStep = PIN_STEP_RPM;
  cfg.pinDirection = PIN_DIR_RPM;
  cfg.pinEnable = PIN_EN_RPM;
  cfg.invertDirection = false;
  cfg.stepPulseUs = 2;
  return cfg;
}

static GaugeMotorTmc2208::Config makeFuelMotorConfig() {
  GaugeMotorTmc2208::Config cfg;
  cfg.pinStep = PIN_STEP_FUEL;
  cfg.pinDirection = PIN_DIR_FUEL;
  cfg.pinEnable = PIN_EN_FUEL;
  cfg.invertDirection = false;
  cfg.stepPulseUs = 2;
  return cfg;
}

static GaugeMotorTmc2208::Config makeTempMotorConfig() {
  GaugeMotorTmc2208::Config cfg;
  cfg.pinStep = PIN_STEP_TEMP;
  cfg.pinDirection = PIN_DIR_TEMP;
  cfg.pinEnable = PIN_EN_TEMP;
  cfg.invertDirection = false;
  cfg.stepPulseUs = 2;
  return cfg;
}

static SpeedGauge::Config makeSpeedGaugeConfig() {
  SpeedGauge::Config cfg;
  cfg.maxSpeedKmh = SPEED_MAX_KMH;
  cfg.minSteps = 0;
  cfg.maxSteps = SPEED_RANGE_STEPS_0_TO_MAX;
  cfg.normalStepsPerSec = SPEED_NORMAL_STEPS_PER_SEC;
  cfg.fastStepsPerSec = SPEED_FAST_STEPS_PER_SEC;
  cfg.calibrationRiseStepsPerSec = DEFAULT_CALIBRATION_RISE_STEPS_PER_SEC;
  cfg.calibrationFallStepsPerSec = DEFAULT_CALIBRATION_FALL_STEPS_PER_SEC;
  return cfg;
}

static RpmGauge::Config makeRpmGaugeConfig() {
  RpmGauge::Config cfg;
  cfg.maxRpm = RPM_MAX_VALUE;
  cfg.minSteps = 0;
  cfg.maxSteps = RPM_RANGE_STEPS_0_TO_MAX;
  cfg.normalStepsPerSec = RPM_NORMAL_STEPS_PER_SEC;
  cfg.fastStepsPerSec = RPM_FAST_STEPS_PER_SEC;
  cfg.calibrationRiseStepsPerSec = DEFAULT_CALIBRATION_RISE_STEPS_PER_SEC;
  cfg.calibrationFallStepsPerSec = DEFAULT_CALIBRATION_FALL_STEPS_PER_SEC;
  return cfg;
}

static FuelGauge::Config makeFuelGaugeConfig() {
  FuelGauge::Config cfg;
  cfg.minFuelLevel = FUEL_MIN_LEVEL;
  cfg.maxFuelLevel = FUEL_MAX_LEVEL;
  cfg.minSteps = 0;
  cfg.maxSteps = FUEL_RANGE_STEPS_0_TO_MAX;
  cfg.normalStepsPerSec = FUEL_NORMAL_STEPS_PER_SEC;
  cfg.fastStepsPerSec = FUEL_FAST_STEPS_PER_SEC;
  cfg.calibrationRiseStepsPerSec = DEFAULT_CALIBRATION_RISE_STEPS_PER_SEC;
  cfg.calibrationFallStepsPerSec = DEFAULT_CALIBRATION_FALL_STEPS_PER_SEC;
  return cfg;
}

static TireTempGauge::Config makeTireTempGaugeConfig() {
  TireTempGauge::Config cfg;
  cfg.minTemp = TIRE_TEMP_MIN_C;
  cfg.maxTemp = TIRE_TEMP_MAX_C;
  cfg.minSteps = 0;
  cfg.maxSteps = TEMP_RANGE_STEPS_0_TO_MAX;
  cfg.normalStepsPerSec = TEMP_NORMAL_STEPS_PER_SEC;
  cfg.fastStepsPerSec = TEMP_FAST_STEPS_PER_SEC;
  cfg.calibrationRiseStepsPerSec = DEFAULT_CALIBRATION_RISE_STEPS_PER_SEC;
  cfg.calibrationFallStepsPerSec = DEFAULT_CALIBRATION_FALL_STEPS_PER_SEC;
  return cfg;
}

static AppConfig makeAppConfig() {
  AppConfig cfg;

  cfg.udpPort = UDP_PORT;

  return cfg;
}

// ===== Configs estáticas =====
static const GaugeMotorTmc2208::Config speedMotorCfg = makeSpeedMotorConfig();
static const GaugeMotorTmc2208::Config rpmMotorCfg   = makeRpmMotorConfig();
static const GaugeMotorTmc2208::Config fuelMotorCfg  = makeFuelMotorConfig();
static const GaugeMotorTmc2208::Config tempMotorCfg  = makeTempMotorConfig();

static const SpeedGauge::Config speedGaugeCfg       = makeSpeedGaugeConfig();
static const RpmGauge::Config rpmGaugeCfg           = makeRpmGaugeConfig();
static const FuelGauge::Config fuelGaugeCfg         = makeFuelGaugeConfig();
static const TireTempGauge::Config tireTempGaugeCfg = makeTireTempGaugeConfig();

// ===== Objetos =====
static GaugeMotorTmc2208 speedMotor(speedMotorCfg);
static GaugeMotorTmc2208 rpmMotor(rpmMotorCfg);
static GaugeMotorTmc2208 fuelMotor(fuelMotorCfg);
static GaugeMotorTmc2208 tempMotor(tempMotorCfg);

static SpeedGauge speedGauge(speedMotor, speedGaugeCfg);
static RpmGauge rpmGauge(rpmMotor, rpmGaugeCfg);
static FuelGauge fuelGauge(fuelMotor, fuelGaugeCfg);
static TireTempGauge tireTempGauge(tempMotor, tireTempGaugeCfg);

static App app(
  telemetry,
  ui,
  speedGauge,
  rpmGauge,
  fuelGauge,
  tireTempGauge
);

void setup() {
  wifiPortal.begin();

  if (wifiPortal.isConnected()) {
    app.begin(makeAppConfig());
    appStarted = true;
  }
}

void loop() {
  if (wifiPortal.isPortalActive()) {
    wifiPortal.tick();
    return;
  }

  if (!appStarted && wifiPortal.isConnected()) {
    app.begin(makeAppConfig());
    appStarted = true;
  }

  if (appStarted) {
    app.tick();
  }
}
