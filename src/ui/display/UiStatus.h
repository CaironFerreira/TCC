#pragma once

#include <Arduino.h>

struct UiStatus {
  String ssid;
  bool wifiConnected = false;
  String ip;
  bool telemetryValid = false;
  float speedKmh = 0.0f;
  int rpm = 0;
  int gear = 0;
  int lapNumber = 0;
  int racePosition = 0;
  float fuel = 0.0f;
  float tireTempAvg = 0.0f;
  float tireTempFL = 0.0f;
  float tireTempFR = 0.0f;
  float tireTempRL = 0.0f;
  float tireTempRR = 0.0f;
};
