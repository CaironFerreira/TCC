#include <stdint.h>
#include <unity.h>

#include "application/App.h"
#include "support/TestDoubles.h"

using test_support::FakeButtonInput;
using test_support::FakeClock;
using test_support::FakeInstrumentCluster;
using test_support::FakeStatusDisplay;
using test_support::FakeTelemetrySource;
using test_support::FakeWifiConfigPortal;

namespace {

static const uint32_t HOST_ATTACH_DELAY_MS = 8000;

AppConfig makeConfig() {
  AppConfig cfg;
  cfg.udpPort = 4242;
  cfg.displayLayout = 1;
  return cfg;
}

void test_smoke_boots_runtime_and_processes_tick() {
  FakeWifiConfigPortal wifi;
  FakeButtonInput button;
  FakeClock clock;
  FakeTelemetrySource telemetry;
  FakeStatusDisplay display;
  FakeInstrumentCluster instruments;
  App app(wifi, button, clock, telemetry, display, instruments);

  wifi.connected = true;
  telemetry.validTelemetry = true;
  telemetry.anySignal = true;
  telemetry.status = TelemetryInputStatus::PacketReceived;
  telemetry.speed = 80.0f;
  telemetry.rpmValue = 3500.0f;
  telemetry.fuelValue = 0.4f;
  telemetry.tempAvgValue = 75.0f;

  app.begin(makeConfig());
  clock.setMs(100);
  clock.setMicros(100000);
  app.tick();

  TEST_ASSERT_TRUE(telemetry.beginCalled);
  TEST_ASSERT_EQUAL_INT(1, telemetry.tickCalls);
  TEST_ASSERT_EQUAL_INT(1, instruments.beginCalls);
  TEST_ASSERT_EQUAL_INT(1, instruments.speedCalls);
  TEST_ASSERT_EQUAL_INT(1, instruments.rpmCalls);
  TEST_ASSERT_EQUAL_INT(1, instruments.fuelCalls);
  TEST_ASSERT_EQUAL_INT(1, instruments.tireTempCalls);
  TEST_ASSERT_EQUAL_INT(2, display.tickCalls);
}

void test_smoke_shows_portal_screen_when_wifi_portal_is_active() {
  FakeWifiConfigPortal wifi;
  FakeButtonInput button;
  FakeClock clock;
  FakeTelemetrySource telemetry;
  FakeStatusDisplay display;
  FakeInstrumentCluster instruments;
  App app(wifi, button, clock, telemetry, display, instruments);

  wifi.portalActive = true;

  app.begin(makeConfig());

  TEST_ASSERT_EQUAL_INT(1, display.showMessageCalls);
  TEST_ASSERT_EQUAL_STRING("Wi-Fi SimHub", display.lastMessage1);
  TEST_ASSERT_FALSE(telemetry.beginCalled);
}

}  // namespace

void setUp() {}
void tearDown() {}

#ifdef ARDUINO
#include <Arduino.h>
void setup() {
  // Give PlatformIO time to attach the serial test monitor after upload.
  delay(HOST_ATTACH_DELAY_MS);
  UNITY_BEGIN();
  RUN_TEST(test_smoke_boots_runtime_and_processes_tick);
  RUN_TEST(test_smoke_shows_portal_screen_when_wifi_portal_is_active);
  UNITY_END();
}

void loop() {}
#else
int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_smoke_boots_runtime_and_processes_tick);
  RUN_TEST(test_smoke_shows_portal_screen_when_wifi_portal_is_active);
  return UNITY_END();
}
#endif
