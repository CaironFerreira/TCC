#pragma once
#include <stdint.h>

struct TelemetryFrame {
  bool valid = false;
  uint32_t timestampMs = 0;

  // MVP atual
  float speedKmh = 0.0f;
  uint16_t rpm = 0;
  int8_t gear = 0; // -1 = R, 0 = N, 1.. = marchas

  // novos dados
  float fuel = 0.0f;

  float tireTempFL = 0.0f;
  float tireTempFR = 0.0f;
  float tireTempRL = 0.0f;
  float tireTempRR = 0.0f;

  float tireTempAvg = 0.0f;

  void invalidate() {
    valid = false;

    speedKmh = 0.0f;
    rpm = 0;
    gear = 0;

    fuel = 0.0f;

    tireTempFL = 0.0f;
    tireTempFR = 0.0f;
    tireTempRL = 0.0f;
    tireTempRR = 0.0f;

    tireTempAvg = 0.0f;
  }
};