#include "ui/display/Layout0Basic.h"
#include <cstdio>
#include <cstring>

void Layout0Basic::begin(TFT_eSPI& display) {
  _display = &display;
  _display->fillScreen(TFT_BLACK);
  _display->setTextColor(TFT_WHITE, TFT_BLACK);
  _display->setTextSize(2);

  const int16_t leftX = 18;

  _display->drawString("SPD:", leftX, 0);
  _display->drawString("RPM:", leftX, 30);
  _display->drawString("FUEL:", leftX, 60);
  _display->drawString("TEMP:", leftX, 90);
}

void Layout0Basic::drawField(int x, int y, const char* value) {
  if (_display == nullptr) {
    return;
  }

  _display->fillRect(x, y, 120, 20, TFT_BLACK);
  _display->drawString(value, x, y);
}

void Layout0Basic::render(const UiStatus& s, bool forceAll, unsigned long now) {
  (void)now;

  if (_display == nullptr) {
    return;
  }

  char buf[16];

  snprintf(buf, sizeof(buf), "%.0f", s.speedKmh);
  if (forceAll || strcmp(buf, _lastSpeed) != 0) {
    strncpy(_lastSpeed, buf, sizeof(_lastSpeed) - 1);
    _lastSpeed[sizeof(_lastSpeed) - 1] = '\0';
    drawField(100, 0, _lastSpeed);
  }

  snprintf(buf, sizeof(buf), "%d", s.rpm);
  if (forceAll || strcmp(buf, _lastRpm) != 0) {
    strncpy(_lastRpm, buf, sizeof(_lastRpm) - 1);
    _lastRpm[sizeof(_lastRpm) - 1] = '\0';
    drawField(100, 30, _lastRpm);
  }

  snprintf(buf, sizeof(buf), "%.1f", s.fuel);
  if (forceAll || strcmp(buf, _lastFuel) != 0) {
    strncpy(_lastFuel, buf, sizeof(_lastFuel) - 1);
    _lastFuel[sizeof(_lastFuel) - 1] = '\0';
    drawField(100, 60, _lastFuel);
  }

  snprintf(buf, sizeof(buf), "%.1f", s.tireTempAvg);
  if (forceAll || strcmp(buf, _lastTemp) != 0) {
    strncpy(_lastTemp, buf, sizeof(_lastTemp) - 1);
    _lastTemp[sizeof(_lastTemp) - 1] = '\0';
    drawField(100, 90, _lastTemp);
  }
}
