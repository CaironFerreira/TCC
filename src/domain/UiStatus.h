#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "domain/TelemetryInputStatus.h"

struct UiStatus {
  static constexpr size_t SSID_CAPACITY = 33;
  static constexpr size_t IP_CAPACITY = 16;

  char ssid[SSID_CAPACITY] = "";
  bool wifiConnected = false;
  char ip[IP_CAPACITY] = "";
  bool telemetryValid = false;
  bool telemetrySignalPresent = false;
  TelemetryInputStatus telemetryInputStatus = TelemetryInputStatus::Idle;
  uint32_t telemetryDiscardedPackets = 0;
  uint32_t telemetryReceiveErrors = 0;
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

  void setSsid(const char* value) {
    copyText(ssid, sizeof(ssid), value);
  }

  void setIp(const char* value) {
    copyText(ip, sizeof(ip), value);
  }

private:
  static void copyText(char* target, size_t targetSize, const char* value) {
    if (target == nullptr || targetSize == 0) {
      return;
    }

    if (value == nullptr) {
      target[0] = '\0';
      return;
    }

    strncpy(target, value, targetSize - 1);
    target[targetSize - 1] = '\0';
  }
};
