#include "ui/display/Layout1Minimal.h"
#include <cstdio>
#include <cstring>

void Layout1Minimal::begin(TFT_eSPI& display) {
  _display = &display;

  _display->fillScreen(TFT_BLACK);
  _display->setTextColor(TFT_WHITE, TFT_BLACK);
  _display->setTextDatum(TL_DATUM);
}

void Layout1Minimal::clearValueBox(int x, int y, int w, int h) {
  if (_display == nullptr) return;
  _display->fillRect(x, y, w, h, TFT_BLACK);
}

void Layout1Minimal::formatGear(int gear, char* out, size_t outSize) const {
  if (out == nullptr || outSize == 0) return;

  if (gear < 0) {
    snprintf(out, outSize, "R");
  } else if (gear == 0) {
    snprintf(out, outSize, "--");
  } else {
    snprintf(out, outSize, "%d", gear);
  }
}

void Layout1Minimal::render(const UiStatus& s, bool forceAll, unsigned long now) {
  (void)now;

  if (_display == nullptr) return;

  char buf[32];

  const int16_t w = _display->width();
  const int16_t h = _display->height();
  const int16_t centerX = w / 2;
  const int16_t speedX = 14;
  const int16_t rightX = w - 32;
  const int16_t topY = 30;
  const int16_t bottomY = h - 61;

  // marcha grande no centro
  formatGear(s.gear, buf, sizeof(buf));
  if (forceAll || strcmp(buf, _lastGear) != 0) {
    strncpy(_lastGear, buf, sizeof(_lastGear) - 1);
    _lastGear[sizeof(_lastGear) - 1] = '\0';

    clearValueBox(0, 60, w, 90);
    _display->setTextSize(5);
    _display->setTextColor(TFT_WHITE, TFT_BLACK);
    _display->drawCentreString(_lastGear, centerX, 75, 2);
  }

  // combustível
  float fuelPercent = s.fuel;
  if (fuelPercent <= 1.0f) {
    fuelPercent *= 100.0f;
  }
  if (fuelPercent < 0.0f) {
    fuelPercent = 0.0f;
  } else if (fuelPercent > 100.0f) {
    fuelPercent = 100.0f;
  }

  snprintf(buf, sizeof(buf), "FUEL %.0f%%", fuelPercent);
  if (forceAll || strcmp(buf, _lastFuel) != 0) {
    strncpy(_lastFuel, buf, sizeof(_lastFuel) - 1);
    _lastFuel[sizeof(_lastFuel) - 1] = '\0';

    clearValueBox(w / 2 - 70, topY, w / 2 + 70, 24);
    _display->setTextSize(2);
    _display->drawRightString(_lastFuel, rightX, topY, 2);
  }

  // velocidade
  snprintf(buf, sizeof(buf), "SPD %.0f", s.speedKmh);
  if (forceAll || strcmp(buf, _lastSpeed) != 0) {
    strncpy(_lastSpeed, buf, sizeof(_lastSpeed) - 1);
    _lastSpeed[sizeof(_lastSpeed) - 1] = '\0';

    clearValueBox(0, bottomY - 2, w / 2, 34);
    _display->setTextSize(2);
    _display->drawString(_lastSpeed, speedX, bottomY, 2);
  }

  // rpm
  snprintf(buf, sizeof(buf), "RPM %d", s.rpm);
  if (forceAll || strcmp(buf, _lastRpm) != 0) {
    strncpy(_lastRpm, buf, sizeof(_lastRpm) - 1);
    _lastRpm[sizeof(_lastRpm) - 1] = '\0';

    clearValueBox(w / 2, bottomY - 2, w / 2, 34);
    _display->setTextSize(2);
    _display->drawRightString(_lastRpm, rightX, bottomY, 2);
  }
}
