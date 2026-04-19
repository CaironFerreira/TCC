#include "ui/display/Layout0Basic.h"
#include <cstdio>
#include <cstring>

void Layout0Basic::begin(TFT_eSPI& display) {
  _display = &display;
  _display->fillScreen(TFT_BLACK);
  _display->setTextColor(TFT_WHITE, TFT_BLACK);
  _display->setTextDatum(TL_DATUM);
}

void Layout0Basic::clearValueBox(int x, int y, int w, int h) {
  if (_display == nullptr) {
    return;
  }

  _display->fillRect(x, y, w, h, TFT_BLACK);
}

void Layout0Basic::formatGear(int gear, char* out, size_t outSize) const {
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

void Layout0Basic::render(const UiStatus& s, bool forceAll, unsigned long now) {
  (void)now;

  if (_display == nullptr) {
    return;
  }

  const int16_t w = _display->width();
  const int16_t h = _display->height();
  const int16_t centerX = w / 2;
  const int16_t marginX = 34;
  const int16_t topY = 30;
  const int16_t gearY = (h / 2) - 54;

  char buf[16];

  if (s.racePosition > 0) {
    snprintf(buf, sizeof(buf), "POS %d", s.racePosition);
  } else {
    snprintf(buf, sizeof(buf), "POS --");
  }
  if (forceAll || strcmp(buf, _lastPosition) != 0) {
    strncpy(_lastPosition, buf, sizeof(_lastPosition) - 1);
    _lastPosition[sizeof(_lastPosition) - 1] = '\0';

    clearValueBox(marginX, topY, 110, 28);
    _display->setTextSize(2);
    _display->drawString(_lastPosition, marginX, topY, 2);
  }

  if (s.telemetryValid) {
    snprintf(buf, sizeof(buf), "LAP %d", s.lapNumber + 1);
  } else {
    snprintf(buf, sizeof(buf), "LAP --");
  }
  if (forceAll || strcmp(buf, _lastLap) != 0) {
    strncpy(_lastLap, buf, sizeof(_lastLap) - 1);
    _lastLap[sizeof(_lastLap) - 1] = '\0';

    clearValueBox(w - marginX - 110, topY, 110, 28);
    _display->setTextSize(2);
    _display->drawRightString(_lastLap, w - marginX, topY, 2);
  }

  formatGear(s.gear, buf, sizeof(buf));
  if (forceAll || strcmp(buf, _lastGear) != 0) {
    strncpy(_lastGear, buf, sizeof(_lastGear) - 1);
    _lastGear[sizeof(_lastGear) - 1] = '\0';

    clearValueBox(0, gearY, w, 150);
    _display->setTextSize(8);
    _display->drawCentreString(_lastGear, centerX, gearY + 4, 2);
  }
}
