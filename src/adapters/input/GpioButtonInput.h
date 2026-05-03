#pragma once

#include <Arduino.h>
#include "ports/IButtonInput.h"

class GpioButtonInput : public IButtonInput {
public:
  struct Config {
    int pin = -1;
    bool activeLow = true;
    bool useInternalPullup = true;
    bool useInternalPulldown = false;
    uint32_t debounceMs = 40;
  };

  explicit GpioButtonInput(const Config& cfg);

  void begin() override;
  void tick(uint32_t nowMs) override;
  bool wasPressed() override;

private:
  bool readPressed() const;

  Config _cfg;

  bool _stablePressed = false;
  bool _lastRawPressed = false;
  bool _pressedEvent = false;
  uint32_t _lastChangeMs = 0;
};
