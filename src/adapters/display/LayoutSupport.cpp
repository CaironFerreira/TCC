#include "adapters/display/LayoutSupport.h"
#include <cstdio>
#include <cstring>

namespace {

void copyText(char* target, size_t targetSize, const char* source) {
  if (target == nullptr || targetSize == 0) {
    return;
  }

  if (source == nullptr) {
    target[0] = '\0';
    return;
  }

  strncpy(target, source, targetSize - 1);
  target[targetSize - 1] = '\0';
}

void formatTelemetryBadge(const UiStatus& status, char* out, size_t outSize) {
  if (out == nullptr || outSize == 0) {
    return;
  }

  if (!status.telemetrySignalPresent) {
    snprintf(out, outSize, "TEL SEM SINAL");
    return;
  }

  switch (status.telemetryInputStatus) {
    case TelemetryInputStatus::ReceiveError:
      snprintf(out, outSize, "TEL ERRO %lu", (unsigned long)status.telemetryReceiveErrors);
      return;
    case TelemetryInputStatus::PacketDiscarded:
      snprintf(out, outSize, "TEL DESC %lu", (unsigned long)status.telemetryDiscardedPackets);
      return;
    case TelemetryInputStatus::PacketReceived:
      if (status.telemetryValid) {
        snprintf(out, outSize, "TEL OK");
      } else {
        snprintf(out, outSize, "TEL INVALIDA");
      }
      return;
    case TelemetryInputStatus::Idle:
      snprintf(out, outSize, "TEL INICIANDO");
      return;
    case TelemetryInputStatus::NoPacket:
    default:
      if (status.telemetryValid) {
        snprintf(out, outSize, "TEL OK");
      } else {
        snprintf(out, outSize, "TEL AGUARDANDO");
      }
      return;
  }
}

uint16_t telemetryBadgeColor(const UiStatus& status) {
  if (!status.telemetrySignalPresent) {
    return TFT_RED;
  }

  switch (status.telemetryInputStatus) {
    case TelemetryInputStatus::ReceiveError:
      return TFT_RED;
    case TelemetryInputStatus::PacketDiscarded:
      return TFT_YELLOW;
    case TelemetryInputStatus::PacketReceived:
      return status.telemetryValid ? TFT_GREEN : TFT_CYAN;
    case TelemetryInputStatus::Idle:
      return TFT_CYAN;
    case TelemetryInputStatus::NoPacket:
    default:
      return status.telemetryValid ? TFT_GREEN : TFT_DARKGREY;
  }
}

}

namespace LayoutSupport {

void clearValueBox(TFT_eSPI* display, int x, int y, int w, int h) {
  if (display == nullptr) {
    return;
  }

  display->fillRect(x, y, w, h, TFT_BLACK);
}

void formatGear(int gear, char* out, size_t outSize) {
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

int fuelPercent(float fuel) {
  float percent = fuel;
  if (percent <= 1.0f) {
    percent *= 100.0f;
  }

  if (percent < 0.0f) {
    percent = 0.0f;
  } else if (percent > 100.0f) {
    percent = 100.0f;
  }

  return static_cast<int>(percent + 0.5f);
}

void drawTelemetryBadge(TFT_eSPI& display,
                        const UiStatus& status,
                        bool forceAll,
                        char* lastText,
                        size_t lastTextSize,
                        uint16_t& lastColor) {
  char badge[24];
  formatTelemetryBadge(status, badge, sizeof(badge));
  const uint16_t color = telemetryBadgeColor(status);

  if (!forceAll && lastText != nullptr && strcmp(badge, lastText) == 0 && color == lastColor) {
    return;
  }

  copyText(lastText, lastTextSize, badge);
  lastColor = color;

  const int16_t width = display.width();
  display.fillRect(width - 140, 2, 138, 18, TFT_BLACK);
  display.setTextDatum(TR_DATUM);
  display.setTextColor(color, TFT_BLACK);
  display.setTextSize(1);
  display.drawString(badge, width - 4, 5, 2);
  display.setTextDatum(TL_DATUM);
}

}
