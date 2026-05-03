#pragma once

#include <stdint.h>
#include <string>

#include "Arduino.h"

static const uint16_t TFT_BLACK = 0x0000;
static const uint16_t TFT_WHITE = 0xFFFF;
static const uint16_t TFT_RED = 0xF800;
static const uint16_t TFT_GREEN = 0x07E0;
static const uint16_t TFT_YELLOW = 0xFFE0;
static const uint16_t TFT_CYAN = 0x07FF;
static const uint16_t TFT_DARKGREY = 0x7BEF;

static const uint8_t TL_DATUM = 0;
static const uint8_t TR_DATUM = 1;
static const uint8_t MC_DATUM = 2;

namespace tft_stub {

struct State {
  uint32_t initCalls = 0;
  uint32_t setRotationCalls = 0;
  uint32_t fillScreenCalls = 0;
  uint32_t fillRectCalls = 0;
  uint32_t drawRectCalls = 0;
  uint32_t drawRoundRectCalls = 0;
  uint32_t fillRoundRectCalls = 0;
  uint32_t drawCentreStringCalls = 0;
  uint32_t drawRightStringCalls = 0;
  uint32_t drawStringCalls = 0;
  uint32_t setTextDatumCalls = 0;
  uint32_t setTextColorCalls = 0;
  uint32_t setTextSizeCalls = 0;
  int16_t width = 320;
  int16_t height = 240;
  uint8_t lastDatum = TL_DATUM;
  uint8_t lastTextSize = 1;
  uint16_t lastTextColor = TFT_WHITE;
  uint16_t lastBackgroundColor = TFT_BLACK;
  std::string lastDrawnText;
};

inline State& state() {
  static State value;
  return value;
}

inline void reset() {
  state() = State{};
}

}  // namespace tft_stub

class TFT_eSPI {
public:
  void init() {
    ++tft_stub::state().initCalls;
  }

  void setRotation(uint8_t) {
    ++tft_stub::state().setRotationCalls;
  }

  int16_t width() const {
    return tft_stub::state().width;
  }

  int16_t height() const {
    return tft_stub::state().height;
  }

  void fillScreen(uint16_t) {
    ++tft_stub::state().fillScreenCalls;
  }

  void fillRect(int32_t, int32_t, int32_t, int32_t, uint16_t) {
    ++tft_stub::state().fillRectCalls;
  }

  void drawRect(int32_t, int32_t, int32_t, int32_t, uint16_t) {
    ++tft_stub::state().drawRectCalls;
  }

  void drawRoundRect(int32_t, int32_t, int32_t, int32_t, int32_t, uint16_t) {
    ++tft_stub::state().drawRoundRectCalls;
  }

  void fillRoundRect(int32_t, int32_t, int32_t, int32_t, int32_t, uint16_t) {
    ++tft_stub::state().fillRoundRectCalls;
  }

  void setTextDatum(uint8_t datum) {
    ++tft_stub::state().setTextDatumCalls;
    tft_stub::state().lastDatum = datum;
  }

  void setTextColor(uint16_t color, uint16_t background) {
    ++tft_stub::state().setTextColorCalls;
    tft_stub::state().lastTextColor = color;
    tft_stub::state().lastBackgroundColor = background;
  }

  void setTextSize(uint8_t size) {
    ++tft_stub::state().setTextSizeCalls;
    tft_stub::state().lastTextSize = size;
  }

  int16_t drawCentreString(const char* text, int32_t, int32_t, int) {
    ++tft_stub::state().drawCentreStringCalls;
    tft_stub::state().lastDrawnText = text != nullptr ? text : "";
    return 0;
  }

  int16_t drawRightString(const char* text, int32_t, int32_t, int) {
    ++tft_stub::state().drawRightStringCalls;
    tft_stub::state().lastDrawnText = text != nullptr ? text : "";
    return 0;
  }

  int16_t drawString(const char* text, int32_t, int32_t, int) {
    ++tft_stub::state().drawStringCalls;
    tft_stub::state().lastDrawnText = text != nullptr ? text : "";
    return 0;
  }
};
