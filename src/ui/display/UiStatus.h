#pragma once

#include <Arduino.h>

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
