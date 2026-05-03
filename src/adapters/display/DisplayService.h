#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>
#include "ports/IStatusDisplay.h"
#include "adapters/display/DisplayLayoutCatalog.h"
#include "domain/UiStatus.h"

class IDisplayLayout;

class DisplayService : public IStatusDisplay {
public:
  DisplayService(const DisplayLayoutEntry* layouts, size_t layoutCount);

  bool begin(uint8_t layoutId) override;
  bool setLayout(uint8_t layoutId);
  bool nextLayout() override;
  uint8_t currentLayoutId() const { return _currentLayoutId; }
  size_t layoutCount() const;

  void setStatus(const UiStatus& s) override;
  void showMessage(const char* line1,
                   const char* line2,
                   const char* line3,
                   const char* line4 = nullptr) override;
  void tick() override;

private:
  int findLayoutIndex(uint8_t layoutId) const;

private:
  const DisplayLayoutEntry* _layouts = nullptr;
  size_t _layoutCount = 0;
  TFT_eSPI _display;
  UiStatus _status;

  IDisplayLayout* _layout = nullptr;
  bool _displayBegun = false;
  bool _forceRender = true;
  uint8_t _currentLayoutId = 0;

  unsigned long _lastRender = 0;
  char _lastTelemetryBadge[24] = "";
  uint16_t _lastTelemetryBadgeColor = TFT_WHITE;
};
