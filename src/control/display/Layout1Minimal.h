#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "IDisplayLayout.h"
#include "../DisplayService.h"

class Layout1Minimal : public IDisplayLayout {
public:
  void begin(TFT_eSPI& display) override;
  void render(const UiStatus& status, bool forceAll, unsigned long now) override;

private:
  TFT_eSPI* _display = nullptr;

  char _lastGear[8] = "";
  char _lastSpeed[16] = "";
  char _lastRpm[16] = "";
  char _lastFuel[16] = "";

  void clearValueBox(int x, int y, int w, int h);
  void formatGear(int gear, char* out, size_t outSize) const;
};