#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>

struct UiStatus {
  bool wifiConnected = false;
  String ssid = "";
  String ip = "-";
  float speedKmh = 0.0f;
  uint16_t rpm = 0;
  int8_t gear = 0; // -1 = R, 0 = N, 1.. = marchas
};

class DisplayService {
public:
  bool begin(int sdaPin = -1, int sclPin = -1, uint8_t i2cAddr = 0x00);
  void setStatus(const UiStatus& s);
  void tick();

private:
  TFT_eSPI display = TFT_eSPI();

  UiStatus status;
  unsigned long lastDraw = 0;
  const unsigned long drawIntervalMs = 250;
  bool ready = false;

  void draw();
  String gearToString() const;
};