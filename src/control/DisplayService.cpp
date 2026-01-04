#include "DisplayService.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static const int SCREEN_W = 128;
static const int SCREEN_H = 64;

static Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);

bool DisplayService::begin(int sdaPin, int sclPin, uint8_t i2cAddr) {
  Wire.begin(sdaPin, sclPin);

  if (!display.begin(SSD1306_SWITCHCAPVCC, i2cAddr)) {
    ready = false;
    return false;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
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

void DisplayService::draw() {
  display.clearDisplay();

  // Linha 1: Rede conectada
  display.setCursor(0, 0);
  display.print("WiFi: ");
  display.print(status.wifiConnected ? "OK " : "NO ");
  display.print(status.ssid);

  // Linha 2: IP
  display.setCursor(0, 16);
  display.print("IP: ");
  display.print(status.ip);

  // Linha 3: RPM
  display.setCursor(0, 32);
  display.print("RPM: ");
  display.print(status.rpm);

  // Linha 4: Velocidade e marcha
  display.setCursor(0, 48);
  display.print("SPD: ");
  display.print(status.speedKmh, 1);
  display.print(" G:");
  if (status.gear == 0) {
    display.print("R");
  } else {
    display.print(status.gear);
  }


  display.display();
}
