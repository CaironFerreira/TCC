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

class IDisplayLayout;

class DisplayService {
public:
  bool begin(uint8_t layoutId);
  void setStatus(const UiStatus& s);
  void tick();

private:
  TFT_eSPI _display;
  UiStatus _status;

  IDisplayLayout* _layout = nullptr;

  unsigned long _lastRender = 0;
};