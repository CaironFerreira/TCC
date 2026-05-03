#include "adapters/display/DisplayService.h"
#include "adapters/display/IDisplayLayout.h"
#include "adapters/display/LayoutSupport.h"

DisplayService::DisplayService(const DisplayLayoutEntry* layouts, size_t layoutCount)
  : _layouts(layouts), _layoutCount(layoutCount) {}

int DisplayService::findLayoutIndex(uint8_t layoutId) const {
  for (size_t i = 0; i < _layoutCount; ++i) {
    if (_layouts[i].id == layoutId) {
      return static_cast<int>(i);
    }
  }

  return -1;
}

bool DisplayService::begin(uint8_t layoutId) {
  if (!_displayBegun) {
    _display.init();
    _display.setRotation(3);
    _displayBegun = true;
  }

  if (!setLayout(layoutId)) {
    if (_layoutCount == 0) {
      return false;
    }

    return setLayout(_layouts[0].id);
  }

  return true;
}

bool DisplayService::setLayout(uint8_t layoutId) {
  if (!_displayBegun) {
    return false;
  }

  if (_layouts == nullptr || _layoutCount == 0) {
    return false;
  }

  const int index = findLayoutIndex(layoutId);
  if (index < 0) {
    return false;
  }

  _layout = _layouts[index].layout;
  _currentLayoutId = _layouts[index].id;
  _lastRender = 0;
  _forceRender = true;
  _lastTelemetryBadge[0] = '\0';
  _lastTelemetryBadgeColor = TFT_WHITE;

  _layout->begin(_display);
  return true;
}

bool DisplayService::nextLayout() {
  if (!_displayBegun || _layoutCount == 0) {
    return false;
  }

  int index = findLayoutIndex(_currentLayoutId);
  if (index < 0) {
    index = 0;
  } else {
    index = (index + 1) % static_cast<int>(_layoutCount);
  }

  return setLayout(_layouts[index].id);
}

size_t DisplayService::layoutCount() const {
  return _layoutCount;
}

void DisplayService::setStatus(const UiStatus& s) {
  _status = s;
}

void DisplayService::showMessage(const char* line1,
                                 const char* line2,
                                 const char* line3,
                                 const char* line4) {
  const int16_t centerX = _display.width() / 2;

  _display.fillScreen(TFT_BLACK);
  _display.setTextDatum(MC_DATUM);
  _display.setTextColor(TFT_WHITE, TFT_BLACK);

  _display.setTextSize(1);
  _display.drawCentreString(line1 ? line1 : "", centerX, 42, 4);

  _display.drawCentreString(line2 ? line2 : "", centerX, 96, 2);
  _display.drawCentreString(line3 ? line3 : "", centerX, 126, 2);
  _display.drawCentreString(line4 ? line4 : "", centerX, 156, 2);

  _display.setTextDatum(TL_DATUM);
}

void DisplayService::tick() {
  unsigned long now = millis();

  if (_layout == nullptr) return;

  if (!_forceRender && now - _lastRender < 100) return;

  _layout->render(_status, _forceRender, now);
  LayoutSupport::drawTelemetryBadge(
    _display,
    _status,
    _forceRender,
    _lastTelemetryBadge,
    sizeof(_lastTelemetryBadge),
    _lastTelemetryBadgeColor
  );
  _forceRender = false;
  _lastRender = now;
}
