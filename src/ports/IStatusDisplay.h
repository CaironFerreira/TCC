#pragma once

#include <stddef.h>
#include <stdint.h>
#include "domain/UiStatus.h"

class IStatusDisplay {
public:
  virtual ~IStatusDisplay() = default;

  virtual bool begin(uint8_t layoutId) = 0;
  virtual bool nextLayout() = 0;
  virtual void setStatus(const UiStatus& status) = 0;
  virtual void showMessage(const char* line1,
                           const char* line2,
                           const char* line3,
                           const char* line4 = nullptr) = 0;
  virtual void tick() = 0;
};
