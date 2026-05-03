#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "domain/UiStatus.h"

class IDisplayLayout {
public:
  virtual ~IDisplayLayout() = default;

  virtual void begin(TFT_eSPI& display) = 0;

  virtual void render(const UiStatus& status,
                      bool forceAll,
                      unsigned long now) = 0;
};
