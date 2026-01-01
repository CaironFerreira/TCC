// src/main.cpp
#include <Arduino.h>
#include <WiFi.h>

#include "drivers/net/UdpReceiver.h"
#include "decoders/Forza7Decoder.h"
#include "control/TelemetryService.h"
#include "app/App.h"

#include "control/DisplayService.h"

// ===================== CONFIG =====================
static const char* WIFI_SSID = "CAIRON";
static const char* WIFI_PASS = "12345678";

static const uint16_t UDP_PORT = 5300;
static const uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;

static const int OLED_SDA = 21;
static const int OLED_SCL = 22;
static const uint8_t OLED_ADDR = 0x3C;

// ======= Core objects =======
UdpReceiver udp;
Forza7Decoder decoder;
TelemetryService telemetry(udp, decoder);
App app(telemetry);

// UI
DisplayService ui;
UiStatus st;
// ===========================

static void updateUiWifiFields() {
  wl_status_t wst = WiFi.status();
  st.wifiConnected = (wst == WL_CONNECTED);

  if (st.wifiConnected) {
    st.ssid = WiFi.SSID();
    st.ip = WiFi.localIP().toString();
  } else {
    st.ssid = WIFI_SSID;
    st.ip = "-";
  }
}

static bool connectWifiWithTimeout() {
  ui.setStatus(st);
  ui.tick();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(150);

    updateUiWifiFields();
    ui.setStatus(st);
    ui.tick();

    if (millis() - start > WIFI_CONNECT_TIMEOUT_MS) {
      ui.setStatus(st);
      ui.tick();
      return false;
    }
  }

  updateUiWifiFields();
  ui.setStatus(st);
  ui.tick();
  return true;
}

void setup() {

  // OLED
  ui.begin(OLED_SDA, OLED_SCL, OLED_ADDR);
  st.ssid = WIFI_SSID;
  st.ip = "-";
  st.wifiConnected = false;
  ui.setStatus(st);
  ui.tick();

  // Wi-Fi
  connectWifiWithTimeout();

  // UDP
  ui.setStatus(st);
  ui.tick();

  telemetry.begin(UDP_PORT);
  ui.setStatus(st);
  ui.tick();

  app.begin();

  updateUiWifiFields();
  ui.setStatus(st);
  ui.tick();
}

void loop() {
  telemetry.tick();
  app.tick();

  updateUiWifiFields();

  const auto& f = telemetry.lastFrame();
  if (f.valid) {
    st.speedKmh = f.speedKmh;
    st.rpm = f.rpm;
    st.gear = f.gear;
  } else {
    st.speedKmh = 0.0f;
    st.rpm = 0;
    st.gear = 0;
  }

  ui.setStatus(st);
  ui.tick();
}
