#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "ui/display/UiStatus.h"

class IDisplayLayout;

class DisplayService {
public:
  bool begin(uint8_t layoutId);
  bool setLayout(uint8_t layoutId);
  bool nextLayout();
  uint8_t currentLayoutId() const { return _currentLayoutId; }
  size_t layoutCount() const;

  void setStatus(const UiStatus& s);
  void showMessage(const char* line1,
                   const char* line2,
                   const char* line3,
                   const char* line4 = nullptr);
  void tick();

private:
  TFT_eSPI _display;
  UiStatus _status;

  IDisplayLayout* _layout = nullptr;
  bool _displayBegun = false;
  bool _forceRender = true;
  uint8_t _currentLayoutId = 0;

  unsigned long _lastRender = 0;
};
