#pragma once
#include <Arduino.h>

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
  bool begin(int sdaPin = 21, int sclPin = 22, uint8_t i2cAddr = 0x3C);
  void setStatus(const UiStatus& s);
  void tick();

private:
  UiStatus status;
  unsigned long lastDraw = 0;
  const unsigned long drawIntervalMs = 250; // ajuste
  bool ready = false;

  void draw();
};
