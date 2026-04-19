#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "ui/display/IDisplayLayout.h"
#include "ui/display/UiStatus.h"

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

  void clearValueBox(int x, int y, int w, int h);
  void formatGear(int gear, char* out, size_t outSize) const;
  uint16_t fuelColor(int percent) const;
};
