#include "ui/display/Layout2Performance.h"
#include <cstdio>
#include <cstring>

void Layout2Performance::begin(TFT_eSPI& display) {
  _display = &display;
  _display->fillScreen(TFT_BLACK);
  _display->setTextDatum(TL_DATUM);
  _display->setTextColor(TFT_WHITE, TFT_BLACK);

  const int16_t w = _display->width();
  const int16_t h = _display->height();
  const int16_t marginX = 24;

  _display->setTextSize(2);
  _display->drawString("FUEL", marginX, 32, 2);
  _display->drawString("RPM", marginX, h - 82, 2);
  _display->drawRect(marginX, h - 50, w - (marginX * 2), 14, TFT_DARKGREY);

  _lastSpeed[0] = '\0';
  _lastGear[0] = '\0';
  _lastFuel[0] = '\0';
  _lastRpmBar = -1;
}

void Layout2Performance::clearValueBox(int x, int y, int w, int h) {
  if (_display == nullptr) {
    return;
  }

  _display->fillRect(x, y, w, h, TFT_BLACK);
}

void Layout2Performance::formatGear(int gear, char* out, size_t outSize) const {
  if (out == nullptr || outSize == 0) {
    return;
  }

  if (gear < 0) {
    snprintf(out, outSize, "R");
  } else if (gear == 0) {
    snprintf(out, outSize, "--");
  } else {
    snprintf(out, outSize, "%d", gear);
  }
}

uint16_t Layout2Performance::fuelColor(int percent) const {
  if (percent <= 15) {
    return TFT_RED;
  }

  if (percent <= 35) {
    return TFT_YELLOW;
  }

  return TFT_GREEN;
}

void Layout2Performance::render(const UiStatus& s, bool forceAll, unsigned long now) {
  (void)now;

  if (_display == nullptr) {
    return;
  }

  const int16_t w = _display->width();
  const int16_t h = _display->height();
  const int16_t centerX = w / 2;
  const int16_t marginX = 24;
  const int16_t fuelRightX = w - 46;
  char buf[24];

  snprintf(buf, sizeof(buf), "%.0f", s.speedKmh);
  if (forceAll || strcmp(buf, _lastSpeed) != 0) {
    strncpy(_lastSpeed, buf, sizeof(_lastSpeed) - 1);
    _lastSpeed[sizeof(_lastSpeed) - 1] = '\0';

    clearValueBox(0, 64, w - 136, 58);
    _display->setTextSize(2);
    _display->setTextColor(TFT_CYAN, TFT_BLACK);
    _display->drawCentreString(_lastSpeed, centerX, 66, 4);
    _display->setTextColor(TFT_WHITE, TFT_BLACK);
  }

  formatGear(s.gear, buf, sizeof(buf));
  if (forceAll || strcmp(buf, _lastGear) != 0) {
    strncpy(_lastGear, buf, sizeof(_lastGear) - 1);
    _lastGear[sizeof(_lastGear) - 1] = '\0';

    clearValueBox(centerX - 48, 126, 96, 66);
    _display->setTextSize(4);
    _display->setTextColor(TFT_WHITE, TFT_BLACK);
    _display->drawCentreString(_lastGear, centerX, 128, 2);
  }

  float fuel = s.fuel;
  if (fuel <= 1.0f) {
    fuel *= 100.0f;
  }
  if (fuel < 0.0f) {
    fuel = 0.0f;
  } else if (fuel > 100.0f) {
    fuel = 100.0f;
  }

  const int fuelPercent = (int)(fuel + 0.5f);
  snprintf(buf, sizeof(buf), "%d%%", fuelPercent);
  if (forceAll || strcmp(buf, _lastFuel) != 0) {
    strncpy(_lastFuel, buf, sizeof(_lastFuel) - 1);
    _lastFuel[sizeof(_lastFuel) - 1] = '\0';

    clearValueBox(w - 112, 32, 66, 32);
    _display->setTextSize(2);
    _display->setTextColor(fuelColor(fuelPercent), TFT_BLACK);
    _display->drawRightString(_lastFuel, fuelRightX, 32, 2);
    _display->setTextColor(TFT_WHITE, TFT_BLACK);
  }

  const int barX = marginX + 1;
  const int barY = h - 49;
  const int barW = w - (marginX * 2) - 2;
  const int filled = constrain((s.rpm * barW) / 8000, 0, barW);
  if (forceAll || filled != _lastRpmBar) {
    _lastRpmBar = filled;

    _display->fillRect(barX, barY, barW, 12, TFT_BLACK);
    const uint16_t color = (s.rpm > 6500) ? TFT_RED : ((s.rpm > 4500) ? TFT_YELLOW : TFT_GREEN);
    if (filled > 0) {
      _display->fillRect(barX, barY, filled, 12, color);
    }
  }
}
