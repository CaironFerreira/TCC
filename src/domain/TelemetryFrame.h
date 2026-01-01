#pragma once
#include <stdint.h>

struct TelemetryFrame {
  bool valid = false;
  uint32_t timestampMs = 0;

  // MVP
  float speedKmh = 0.0f;
  uint16_t rpm = 0;
  int8_t gear = 0; // -1 = R, 0 = N, 1.. = marchas

  void invalidate() {
    valid = false;
    speedKmh = 0.0f;
    rpm = 0;
    gear = 0;
  }
};
