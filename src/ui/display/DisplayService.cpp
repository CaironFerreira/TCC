#include "ui/display/DisplayService.h"
#include "ui/display/Layout0Basic.h"
#include "ui/display/Layout1Minimal.h"
#include "ui/display/Layout2Performance.h"
#include "ui/display/Layout3Tires.h"

namespace {

static Layout0Basic g_layout0;
static Layout1Minimal g_layout1;
static Layout2Performance g_layout2;
static Layout3Tires g_layout3;

struct LayoutEntry {
  uint8_t id;
  IDisplayLayout* layout;
};

static LayoutEntry g_layouts[] = {
  {0, &g_layout0},
  {1, &g_layout1},
  {2, &g_layout2},
  {3, &g_layout3}
};

static constexpr size_t LAYOUT_COUNT = sizeof(g_layouts) / sizeof(g_layouts[0]);

static int findLayoutIndex(uint8_t layoutId) {
  for (size_t i = 0; i < LAYOUT_COUNT; ++i) {
    if (g_layouts[i].id == layoutId) {
      return static_cast<int>(i);
    }
  }

  return -1;
}

}

bool DisplayService::begin(uint8_t layoutId) {
  if (!_displayBegun) {
    _display.init();
    _display.setRotation(3);
    _displayBegun = true;
  }

  if (!setLayout(layoutId)) {
    return setLayout(g_layouts[0].id);
  }

  return true;
}

bool DisplayService::setLayout(uint8_t layoutId) {
  if (!_displayBegun) {
    return false;
  }

  const int index = findLayoutIndex(layoutId);
  if (index < 0) {
    return false;
  }

  _layout = g_layouts[index].layout;
  _currentLayoutId = g_layouts[index].id;
  _lastRender = 0;
  _forceRender = true;

  _layout->begin(_display);
  return true;
}

bool DisplayService::nextLayout() {
  if (!_displayBegun || LAYOUT_COUNT == 0) {
    return false;
  }

  int index = findLayoutIndex(_currentLayoutId);
  if (index < 0) {
    index = 0;
  } else {
    index = (index + 1) % static_cast<int>(LAYOUT_COUNT);
  }

  return setLayout(g_layouts[index].id);
}

size_t DisplayService::layoutCount() const {
  return LAYOUT_COUNT;
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
  _forceRender = false;
  _lastRender = now;
}
