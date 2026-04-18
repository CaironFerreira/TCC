#pragma once

#include <stdint.h>

class IButtonInput {
public:
  virtual ~IButtonInput() = default;

  virtual void begin() = 0;
  virtual void tick(uint32_t nowMs) = 0;
  virtual bool wasPressed() = 0;
};
