#include "ui/display/Layout3Tires.h"
#include <cstdio>
#include <cstring>

void Layout3Tires::begin(TFT_eSPI& display) {
  _display = &display;
  _display->fillScreen(TFT_BLACK);
  _display->setTextDatum(TL_DATUM);
  _display->setTextColor(TFT_WHITE, TFT_BLACK);
  _display->setTextSize(2);
  _display->drawCentreString("PNEUS", (_display->width() / 2) - 6, 34, 2);

  _lastFL[0] = '\0';
  _lastFR[0] = '\0';
  _lastRL[0] = '\0';
  _lastRR[0] = '\0';
}

uint16_t Layout3Tires::tempColor(float tempC) const {
  if (tempC <= 0.0f) {
    return TFT_DARKGREY;
  }

  if (tempC < 60.0f) {
    return TFT_CYAN;
  }

  if (tempC < 90.0f) {
    return TFT_GREEN;
  }

  if (tempC < 110.0f) {
    return TFT_YELLOW;
  }

  return TFT_RED;
}

void Layout3Tires::drawTireValue(int x,
                                 int y,
                                 int w,
                                 int h,
                                 const char* label,
                                 float tempC,
                                 char* last,
                                 size_t lastSize,
                                 bool forceAll) {
  char buf[16];

  if (tempC > 0.0f) {
    snprintf(buf, sizeof(buf), "%s %.0fC", label, tempC);
  } else {
    snprintf(buf, sizeof(buf), "%s --", label);
  }

  if (!forceAll && strcmp(buf, last) == 0) {
    return;
  }

  strncpy(last, buf, lastSize - 1);
  last[lastSize - 1] = '\0';

  _display->fillRoundRect(x, y, w, h, 6, TFT_BLACK);
  _display->drawRoundRect(x, y, w, h, 6, tempColor(tempC));
  _display->setTextColor(tempColor(tempC), TFT_BLACK);
  _display->setTextSize(2);
  _display->drawCentreString(last, x + (w / 2), y + 11, 2);
  _display->setTextColor(TFT_WHITE, TFT_BLACK);
}

void Layout3Tires::render(const UiStatus& s, bool forceAll, unsigned long now) {
  (void)now;

  if (_display == nullptr) {
    return;
  }

  const int16_t w = _display->width();
  const int16_t h = _display->height();
  const int16_t boxW = 92;
  const int16_t boxH = 40;
  const int16_t gap = 18;
  const int16_t totalW = (boxW * 2) + gap;
  const int16_t leftX = ((w - totalW) / 2) - 6;
  const int16_t rightX = leftX + boxW + gap;
  const int16_t topY = 74;
  const int16_t bottomY = h - 70;

  drawTireValue(leftX, topY, boxW, boxH, "FL", s.tireTempFL, _lastFL, sizeof(_lastFL), forceAll);
  drawTireValue(rightX, topY, boxW, boxH, "FR", s.tireTempFR, _lastFR, sizeof(_lastFR), forceAll);
  drawTireValue(leftX, bottomY, boxW, boxH, "RL", s.tireTempRL, _lastRL, sizeof(_lastRL), forceAll);
  drawTireValue(rightX, bottomY, boxW, boxH, "RR", s.tireTempRR, _lastRR, sizeof(_lastRR), forceAll);
}
