#pragma once

#include <Arduino.h>
#include "ports/IClock.h"

class ArduinoClock : public IClock {
public:
  uint32_t nowMs() const override { return millis(); }
  uint32_t nowMicros() const override { return micros(); }
};
