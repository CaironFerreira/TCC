#include "Forza7Decoder.h"
#include <Arduino.h>
#include <string.h> // memcpy

static inline float readF32LE(const uint8_t* p) {
  float v;
  memcpy(&v, p, sizeof(float));   // ESP32 é little-endian, então ok
  return v;
}

static inline uint8_t readU8(const uint8_t* p) {
  return *p;
}

bool Forza7Decoder::decode(const uint8_t* data, size_t len, TelemetryFrame& out) {
  // FM7 "Dash" ~ 311 bytes
  if (!data || len < 311) return false;

  out.valid = true;
  out.timestampMs = millis();

  // Offsets (FM7 Dash)
  constexpr size_t OFF_RPM   = 16;   // F32 CurrentEngineRpm
  constexpr size_t OFF_SPEED = 244;  // F32 Speed (m/s)
  constexpr size_t OFF_GEAR  = 307;  // U8 Gear

  const float rpm = readF32LE(data + OFF_RPM);
  const float speedMs = readF32LE(data + OFF_SPEED);
  const uint8_t gear = readU8(data + OFF_GEAR);

  out.rpm = rpm;
  out.speedKmh = speedMs * 3.6f;
  out.gear = (int)gear;

  return true;
}
