#pragma once

#include <stdint.h>

class IClock {
public:
  virtual ~IClock() = default;

  virtual uint32_t nowMs() const = 0;
  virtual uint32_t nowMicros() const = 0;
};
