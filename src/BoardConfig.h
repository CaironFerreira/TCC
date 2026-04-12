#pragma once
#include <Arduino.h>

// ===================== WIFI / REDE =====================

static constexpr uint16_t UDP_PORT = 5300;
static constexpr uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;

// ===================== PINOS DOS MOTORES =====================
// Velocímetro
static constexpr int PIN_STEP_SPEED = 25;
static constexpr int PIN_DIR_SPEED  = 26;
static constexpr int PIN_EN_SPEED   = 27;

// RPM
static constexpr int PIN_STEP_RPM = 12;
static constexpr int PIN_DIR_RPM  = 13;
static constexpr int PIN_EN_RPM   = 14;

// Combustível
static constexpr int PIN_STEP_FUEL = 15;
static constexpr int PIN_DIR_FUEL  = 19;
static constexpr int PIN_EN_FUEL   = 22;

// Temperatura pneus
static constexpr int PIN_STEP_TEMP = 32;
static constexpr int PIN_DIR_TEMP  = 33;
static constexpr int PIN_EN_TEMP   = 17;

// ===================== ESCALAS DOS INSTRUMENTOS =====================
static constexpr int32_t SPEED_RANGE_STEPS_0_TO_MAX = 180;
static constexpr float   SPEED_MAX_KMH              = 220.0f;

static constexpr int32_t RPM_RANGE_STEPS_0_TO_MAX   = 190;
static constexpr float   RPM_MAX_VALUE              = 8000.0f;

static constexpr int32_t FUEL_RANGE_STEPS_0_TO_MAX  = 180;
static constexpr int32_t TEMP_RANGE_STEPS_0_TO_MAX  = 180;

// ===================== VELOCIDADES / CALIBRAÇÃO =====================
static constexpr float DEFAULT_CALIBRATION_RISE_STEPS_PER_SEC = 300.0f;
static constexpr float DEFAULT_CALIBRATION_FALL_STEPS_PER_SEC = 300.0f;

static constexpr float SPEED_NORMAL_STEPS_PER_SEC = 50.0f;
static constexpr float SPEED_FAST_STEPS_PER_SEC   = 500.0f;

static constexpr float RPM_NORMAL_STEPS_PER_SEC = 60.0f;
static constexpr float RPM_FAST_STEPS_PER_SEC   = 300.0f;

static constexpr float FUEL_NORMAL_STEPS_PER_SEC = 120.0f;
static constexpr float FUEL_FAST_STEPS_PER_SEC   = 120.0f;

static constexpr float TEMP_NORMAL_STEPS_PER_SEC = 180.0f;
static constexpr float TEMP_FAST_STEPS_PER_SEC   = 220.0f;

// ===================== LIMITES LÓGICOS =====================
static constexpr float FUEL_MIN_LEVEL = 0.0f;
static constexpr float FUEL_MAX_LEVEL = 1.0f;

static constexpr float TIRE_TEMP_MIN_C = 20.0f;
static constexpr float TIRE_TEMP_MAX_C = 120.0f;