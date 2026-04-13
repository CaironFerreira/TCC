#include "DisplayService.h"
#include "display/Layout0Basic.h"
#include "display/Layout1Minimal.h"

static Layout0Basic g_layout0;
static Layout1Minimal g_layout1;

bool DisplayService::begin(uint8_t layoutId) {
  _display.init();
  _display.setRotation(3);

  switch (layoutId) {
    case 1: _layout = &g_layout1; break;
    default: _layout = &g_layout0; break;
  }

  _layout->begin(_display);
  return true;
}

void DisplayService::setStatus(const UiStatus& s) {
  _status = s;
}

void DisplayService::tick() {
  unsigned long now = millis();

  if (now - _lastRender < 100) return;

  _layout->render(_status, false, now);
  _lastRender = now;
}