#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "ui/display/IDisplayLayout.h"
#include "ui/display/UiStatus.h"

class Layout0Basic : public IDisplayLayout {
public:
  void begin(TFT_eSPI& display) override;
  void render(const UiStatus& status, bool forceAll, unsigned long now) override;

private:
  TFT_eSPI* _display = nullptr;

  char _lastSpeed[16] = "";
  char _lastRpm[16] = "";
  char _lastFuel[16] = "";
  char _lastTemp[16] = "";

  void drawField(int x, int y, const char* value);
};
