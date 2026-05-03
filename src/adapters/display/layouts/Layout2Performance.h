#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "adapters/display/IDisplayLayout.h"
#include "domain/UiStatus.h"

class Layout2Performance : public IDisplayLayout {
public:
  void begin(TFT_eSPI& display) override;
  void render(const UiStatus& status, bool forceAll, unsigned long now) override;

private:
  TFT_eSPI* _display = nullptr;

  char _lastSpeed[16] = "";
  char _lastGear[8] = "";
  char _lastFuel[16] = "";
  int _lastRpmBar = -1;

  uint16_t fuelColor(int percent) const;
};
