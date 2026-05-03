#include "adapters/telemetry/Forza7Decoder.h"
#include <string.h>

static inline float readF32LE(const uint8_t* p) {
  float v;
  memcpy(&v, p, sizeof(float));
  return v;
}

static inline uint8_t readU8(const uint8_t* p) {
  return *p;
}

static inline uint16_t readU16LE(const uint8_t* p) {
  return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

bool Forza7Decoder::decode(const uint8_t* data, size_t len, TelemetryFrame& out) {
  // FM7 / Dash
  if (!data || len < 311) {
    out.invalidate();
    return false;
  }

  // Offsets (FM7 Dash)
  constexpr size_t OFF_RPM          = 16;   // F32 CurrentEngineRpm
  constexpr size_t OFF_SPEED        = 244;  // F32 Speed (m/s)
  constexpr size_t OFF_TIRE_TEMP_FL = 256;  // F32 TireTempFrontLeft
  constexpr size_t OFF_TIRE_TEMP_FR = 260;  // F32 TireTempFrontRight
  constexpr size_t OFF_TIRE_TEMP_RL = 264;  // F32 TireTempRearLeft
  constexpr size_t OFF_TIRE_TEMP_RR = 268;  // F32 TireTempRearRight
  constexpr size_t OFF_FUEL         = 276;  // F32 Fuel
  constexpr size_t OFF_LAP_NUMBER   = 300;  // U16 LapNumber
  constexpr size_t OFF_RACE_POS     = 302;  // U8 RacePosition
  constexpr size_t OFF_GEAR         = 307;  // U8 Gear

  const float rpm = readF32LE(data + OFF_RPM);
  const float speedMs = readF32LE(data + OFF_SPEED);

  const float tireTempFL = readF32LE(data + OFF_TIRE_TEMP_FL);
  const float tireTempFR = readF32LE(data + OFF_TIRE_TEMP_FR);
  const float tireTempRL = readF32LE(data + OFF_TIRE_TEMP_RL);
  const float tireTempRR = readF32LE(data + OFF_TIRE_TEMP_RR);

  const float fuel = readF32LE(data + OFF_FUEL);
  const uint16_t lapNumber = readU16LE(data + OFF_LAP_NUMBER);
  const uint8_t racePosition = readU8(data + OFF_RACE_POS);
  const uint8_t gear = readU8(data + OFF_GEAR);

  out.valid = true;
  out.rpm = (uint16_t)rpm;
  out.speedKmh = speedMs * 3.6f;
  out.gear = (gear == 0) ? -1 : (int8_t)gear;
  out.lapNumber = lapNumber;
  out.racePosition = racePosition;

  out.fuel = fuel;

  out.tireTempFL = tireTempFL;
  out.tireTempFR = tireTempFR;
  out.tireTempRL = tireTempRL;
  out.tireTempRR = tireTempRR;

  out.tireTempAvg =
      (tireTempFL + tireTempFR + tireTempRL + tireTempRR) * 0.25f;

  return true;
}
