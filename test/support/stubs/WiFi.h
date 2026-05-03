#pragma once

#include <stdint.h>

#include "Arduino.h"

typedef int wl_status_t;

static const wl_status_t WL_IDLE_STATUS = 0;
static const wl_status_t WL_NO_SSID_AVAIL = 1;
static const wl_status_t WL_CONNECTED = 3;
static const wl_status_t WL_CONNECT_FAILED = 4;
static const wl_status_t WL_CONNECTION_LOST = 5;
static const wl_status_t WL_DISCONNECTED = 6;

enum WiFiMode_t : uint8_t {
  WIFI_OFF = 0,
  WIFI_STA = 1,
  WIFI_AP = 2,
  WIFI_AP_STA = 3
};

class IPAddress {
public:
  IPAddress() = default;
  IPAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    _bytes[0] = a;
    _bytes[1] = b;
    _bytes[2] = c;
    _bytes[3] = d;
  }

  uint8_t operator[](size_t index) const {
    return (index < 4) ? _bytes[index] : 0;
  }

  String toString() const {
    String text;
    text += String((int)_bytes[0]);
    text += ".";
    text += String((int)_bytes[1]);
    text += ".";
    text += String((int)_bytes[2]);
    text += ".";
    text += String((int)_bytes[3]);
    return text;
  }

private:
  uint8_t _bytes[4] = {0, 0, 0, 0};
};

namespace wifi_stub {

struct State {
  bool persistentEnabled = false;
  bool sleepEnabled = false;
  bool autoReconnectEnabled = false;
  WiFiMode_t mode = WIFI_OFF;
  wl_status_t status = WL_DISCONNECTED;
  IPAddress localIp = IPAddress(192, 168, 0, 55);
  IPAddress softApIp = IPAddress(192, 168, 4, 1);
  String connectedSsid = "";
  String lastBeginSsid = "";
  String lastBeginPassword = "";
  String softApSsid = "";
  String softApPassword = "";
  bool softApResult = true;
  bool softApActive = false;
  uint32_t beginCalls = 0;
  uint32_t disconnectCalls = 0;
  uint32_t softApCalls = 0;
  uint32_t softApDisconnectCalls = 0;
};

inline State& state() {
  static State value;
  return value;
}

inline void reset() {
  state() = State{};
}

inline void setStatus(wl_status_t value) {
  state().status = value;
}

inline void setLocalIp(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  state().localIp = IPAddress(a, b, c, d);
}

inline void setSoftApIp(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  state().softApIp = IPAddress(a, b, c, d);
}

inline void setConnectedSsid(const char* value) {
  state().connectedSsid = value;
}

inline void setSoftApResult(bool value) {
  state().softApResult = value;
}

}  // namespace wifi_stub

class WiFiClass {
public:
  void persistent(bool enabled) {
    wifi_stub::state().persistentEnabled = enabled;
  }

  void setSleep(bool enabled) {
    wifi_stub::state().sleepEnabled = enabled;
  }

  void setAutoReconnect(bool enabled) {
    wifi_stub::state().autoReconnectEnabled = enabled;
  }

  bool mode(WiFiMode_t mode) {
    wifi_stub::state().mode = mode;
    return true;
  }

  bool disconnect(bool, bool) {
    ++wifi_stub::state().disconnectCalls;
    wifi_stub::state().status = WL_DISCONNECTED;
    return true;
  }

  wl_status_t begin(const char* ssid, const char* password) {
    ++wifi_stub::state().beginCalls;
    wifi_stub::state().lastBeginSsid = ssid;
    wifi_stub::state().lastBeginPassword = password;
    return wifi_stub::state().status;
  }

  wl_status_t status() const {
    return wifi_stub::state().status;
  }

  String SSID() const {
    return wifi_stub::state().connectedSsid;
  }

  IPAddress localIP() const {
    return wifi_stub::state().localIp;
  }

  IPAddress softAPIP() const {
    return wifi_stub::state().softApIp;
  }

  bool softAP(const char* ssid, const char* password = nullptr) {
    ++wifi_stub::state().softApCalls;
    wifi_stub::state().softApSsid = ssid;
    wifi_stub::state().softApPassword = password;
    wifi_stub::state().softApActive = wifi_stub::state().softApResult;
    return wifi_stub::state().softApResult;
  }

  bool softAPdisconnect(bool) {
    ++wifi_stub::state().softApDisconnectCalls;
    wifi_stub::state().softApActive = false;
    return true;
  }
};

static WiFiClass WiFi;
