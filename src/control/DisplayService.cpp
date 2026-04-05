#include "DisplayService.h"
#include <stdio.h>
#include <string.h>

enum LineId {
  LINE_WIFI = 0,
  LINE_IP,
  LINE_SPEED,
  LINE_INFO,
  LINE_DEBUG,
  LINE_COUNT
};

static const unsigned long kLineInterval[LINE_COUNT] = {
  1000, // WIFI
  1000, // IP
  100,  // SPEED
  60,   // INFO (RPM + marcha)
  250   // DEBUG
};

bool DisplayService::begin() {
  display.init();
  display.setRotation(3); // horizontal
  display.fillScreen(TFT_BLACK);
  display.setTextDatum(MC_DATUM);
  display.setTextSize(2);

  ready = true;

  for (int i = 0; i < LINE_COUNT; i++) {
    lastLineUpdate[i] = 0;
  }

  lastWifiLine[0]  = '\0';
  lastIpLine[0]    = '\0';
  lastSpeedLine[0] = '\0';
  lastInfoLine[0]  = '\0';
  lastDebugLine[0] = '\0';

  draw(true, 0);
  return true;
}

void DisplayService::setStatus(const UiStatus& s) {
  status = s;
}

void DisplayService::tick() {
  if (!ready) return;

  const unsigned long now = millis();
  draw(false, now);
}

void DisplayService::formatGear(char* out, size_t outSize) const {
  if (out == nullptr || outSize == 0) return;

  if (status.gear == 0) {
    snprintf(out, outSize, "R");
  } else {
    snprintf(out, outSize, "%d", status.gear);
  }
}

void DisplayService::draw(bool forceAll, unsigned long now) {
  const int w  = display.width();
  const int cx = w / 2;
  const int cy = display.height() / 2;

  char wifiLine[40];
  char ipLine[24];
  char speedLine[20];
  char infoLine[32];
  char debugLine[32];
  char gearBuf[8];

  formatGear(gearBuf, sizeof(gearBuf));

  // Linha 1: rede + status
  if (status.ssid.length() == 0) {
    snprintf(wifiLine, sizeof(wifiLine), "SEM REDE %s",
             status.wifiConnected ? "OK" : "OFF");
  } else {
    snprintf(wifiLine, sizeof(wifiLine), "%s %s",
             status.ssid.c_str(),
             status.wifiConnected ? "OK" : "OFF");
  }

  // Linha 2: IP
  snprintf(ipLine, sizeof(ipLine), "%s", status.ip.c_str());

  // Linha 3: velocidade
  snprintf(speedLine, sizeof(speedLine), "%.0f km/h", status.speedKmh);

  // Linha 4: RPM + marcha
  snprintf(infoLine, sizeof(infoLine), "RPM %d   G %s", status.rpm, gearBuf);

  // Linha 5: combustível + temperatura
  snprintf(debugLine, sizeof(debugLine), "F %.1f  T %.1f",
           status.fuel, status.tireTempAvg);

  // WIFI
  if (forceAll ||
      ((now - lastLineUpdate[LINE_WIFI]) >= kLineInterval[LINE_WIFI] &&
       strcmp(wifiLine, lastWifiLine) != 0)) {
    display.fillRect(0, cy - 72, w, 24, TFT_BLACK);
    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.drawString(wifiLine, cx, cy - 60);
    strncpy(lastWifiLine, wifiLine, sizeof(lastWifiLine) - 1);
    lastWifiLine[sizeof(lastWifiLine) - 1] = '\0';
    lastLineUpdate[LINE_WIFI] = now;
  }

  // IP
  if (forceAll ||
      ((now - lastLineUpdate[LINE_IP]) >= kLineInterval[LINE_IP] &&
       strcmp(ipLine, lastIpLine) != 0)) {
    display.fillRect(0, cy - 50, w, 24, TFT_BLACK);
    display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    display.drawString(ipLine, cx, cy - 38);
    strncpy(lastIpLine, ipLine, sizeof(lastIpLine) - 1);
    lastIpLine[sizeof(lastIpLine) - 1] = '\0';
    lastLineUpdate[LINE_IP] = now;
  }

  // SPEED
  if (forceAll ||
      ((now - lastLineUpdate[LINE_SPEED]) >= kLineInterval[LINE_SPEED] &&
       strcmp(speedLine, lastSpeedLine) != 0)) {
    display.fillRect(0, cy - 17, w, 24, TFT_BLACK);
    display.setTextColor(TFT_CYAN, TFT_BLACK);
    display.drawString(speedLine, cx, cy - 5);
    strncpy(lastSpeedLine, speedLine, sizeof(lastSpeedLine) - 1);
    lastSpeedLine[sizeof(lastSpeedLine) - 1] = '\0';
    lastLineUpdate[LINE_SPEED] = now;
  }

  // INFO
  if (forceAll ||
      ((now - lastLineUpdate[LINE_INFO]) >= kLineInterval[LINE_INFO] &&
       strcmp(infoLine, lastInfoLine) != 0)) {
    display.fillRect(0, cy + 10, w, 24, TFT_BLACK);
    display.setTextColor(TFT_YELLOW, TFT_BLACK);
    display.drawString(infoLine, cx, cy + 22);
    strncpy(lastInfoLine, infoLine, sizeof(lastInfoLine) - 1);
    lastInfoLine[sizeof(lastInfoLine) - 1] = '\0';
    lastLineUpdate[LINE_INFO] = now;
  }

  // DEBUG
  if (forceAll ||
      ((now - lastLineUpdate[LINE_DEBUG]) >= kLineInterval[LINE_DEBUG] &&
       strcmp(debugLine, lastDebugLine) != 0)) {
    display.fillRect(0, cy + 36, w, 24, TFT_BLACK);
    display.setTextColor(TFT_GREEN, TFT_BLACK);
    display.drawString(debugLine, cx, cy + 48);
    strncpy(lastDebugLine, debugLine, sizeof(lastDebugLine) - 1);
    lastDebugLine[sizeof(lastDebugLine) - 1] = '\0';
    lastLineUpdate[LINE_DEBUG] = now;
  }
}