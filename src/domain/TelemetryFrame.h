#pragma once
#include <stdint.h>

struct TelemetryFrame {
  bool valid = false;
  uint32_t timestampMs = 0;

  // MVP atual
  float speedKmh = 0.0f;
  uint16_t rpm = 0;
  int8_t gear = 0; // Forza 7: -1 = R, 0 = desconhecido/sem telemetria, 1.. = marchas
  uint16_t lapNumber = 0;
  uint8_t racePosition = 0;

  // novos dados
  float fuel = 0.0f;

  float tireTempFL = 0.0f;
  float tireTempFR = 0.0f;
  float tireTempRL = 0.0f;
  float tireTempRR = 0.0f;

  float tireTempAvg = 0.0f;

  void invalidate() {
    valid = false;
    timestampMs = 0;

    speedKmh = 0.0f;
    rpm = 0;
    gear = 0;
    lapNumber = 0;
    racePosition = 0;

    fuel = 0.0f;

    tireTempFL = 0.0f;
    tireTempFR = 0.0f;
    tireTempRL = 0.0f;
    tireTempRR = 0.0f;

    tireTempAvg = 0.0f;
  }
};
