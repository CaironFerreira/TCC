#pragma once
#include <Arduino.h>
#include <TFT_eSPI.h>

struct UiStatus {
  String ssid;
  bool wifiConnected = false;
  String ip;
  float speedKmh = 0.0f;
  int rpm = 0;
  int gear = 0;
  float fuel = 0.0f;
  float tireTempAvg = 0.0f;
};

class DisplayService {
public:
  bool begin();
  void setStatus(const UiStatus& s);
  void tick();

private:
  void draw(bool forceAll = false, unsigned long now = 0);
  void formatGear(char* out, size_t outSize) const;

  TFT_eSPI display;
  UiStatus status;
  bool ready = false;

  unsigned long lastLineUpdate[5] = {0, 0, 0, 0, 0};

  char lastWifiLine[40]  = {0};
  char lastIpLine[24]    = {0};
  char lastSpeedLine[20] = {0};
  char lastInfoLine[32]  = {0};
  char lastDebugLine[32] = {0};
};