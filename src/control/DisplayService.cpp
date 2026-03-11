#include "DisplayService.h"

bool DisplayService::begin(int sdaPin, int sclPin, uint8_t i2cAddr) {
  (void)sdaPin;
  (void)sclPin;
  (void)i2cAddr;

  display.init();
  display.setRotation(1); // horizontal
  display.fillScreen(TFT_BLACK);
  display.setTextDatum(MC_DATUM);

  ready = true;
  draw();
  return true;
}

void DisplayService::setStatus(const UiStatus& s) {
  status = s;
}

void DisplayService::tick() {
  if (!ready) return;

  unsigned long now = millis();
  if (now - lastDraw < drawIntervalMs) return;

  lastDraw = now;
  draw();
}

String DisplayService::gearToString() const {
  if (status.gear < 0) return "R";
  if (status.gear == 0) return "N";
  return String(status.gear);
}

void DisplayService::draw() {
  display.fillScreen(TFT_BLACK);

  const int cx = display.width() / 2;
  const int cy = display.height() / 2;

  // Linha 1: nome da rede + status
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextSize(2);

  String wifiLine = status.ssid;
  if (wifiLine.length() == 0) wifiLine = "SEM REDE";
  wifiLine += status.wifiConnected ? " OK" : " OFF";
  display.drawString(wifiLine, cx, cy - 60);

  // Linha 2: IP
  display.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  display.setTextSize(2);
  display.drawString(status.ip, cx, cy - 35);

  // Linha 3: velocidade
  display.setTextColor(TFT_CYAN, TFT_BLACK);
  display.setTextSize(3);
  display.drawString(String(status.speedKmh, 0) + " km/h", cx, cy + 5);

  // Linha 4: RPM + marcha
  display.setTextColor(TFT_YELLOW, TFT_BLACK);
  display.setTextSize(2);
  String infoLine = "RPM " + String(status.rpm) + "   G " + gearToString();
  display.drawString(infoLine, cx, cy + 45);
}