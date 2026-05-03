#pragma once

#include <stddef.h>
#include <stdint.h>
#include <TFT_eSPI.h>
#include "domain/UiStatus.h"

namespace LayoutSupport {

void clearValueBox(TFT_eSPI* display, int x, int y, int w, int h);
void formatGear(int gear, char* out, size_t outSize);
int fuelPercent(float fuel);
void drawTelemetryBadge(TFT_eSPI& display,
                        const UiStatus& status,
                        bool forceAll,
                        char* lastText,
                        size_t lastTextSize,
                        uint16_t& lastColor);

}
