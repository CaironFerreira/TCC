#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "ui/display/UiStatus.h"

class IDisplayLayout;

class DisplayService {
public:
  bool begin(uint8_t layoutId);
  void setStatus(const UiStatus& s);
  void tick();

private:
  TFT_eSPI _display;
  UiStatus _status;

  IDisplayLayout* _layout = nullptr;

  unsigned long _lastRender = 0;
};
