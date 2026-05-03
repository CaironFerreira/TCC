#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "adapters/display/IDisplayLayout.h"
#include "domain/UiStatus.h"

class Layout3Tires : public IDisplayLayout {
public:
  void begin(TFT_eSPI& display) override;
  void render(const UiStatus& status, bool forceAll, unsigned long now) override;

private:
  TFT_eSPI* _display = nullptr;

  char _lastFL[16] = "";
  char _lastFR[16] = "";
  char _lastRL[16] = "";
  char _lastRR[16] = "";

  void drawTireValue(int x, int y, int w, int h, const char* label, float tempC, char* last, size_t lastSize, bool forceAll);
  uint16_t tempColor(float tempC) const;
};
