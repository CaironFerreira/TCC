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

AppConfig makeConfig() {
  AppConfig cfg;
  cfg.udpPort = 4242;
  cfg.displayLayout = 2;
  return cfg;
}

void seedTelemetry(FakeTelemetrySource& telemetry) {
  telemetry.validTelemetry = true;
  telemetry.anySignal = true;
  telemetry.status = TelemetryInputStatus::PacketReceived;
  telemetry.discardedPackets = 3;
  telemetry.receiveErrors = 1;
  telemetry.speed = 187.5f;
  telemetry.rpmValue = 6150.0f;
  telemetry.gearValue = 4;
  telemetry.lapValue = 8;
  telemetry.positionValue = 1;
  telemetry.fuelValue = 0.55f;
  telemetry.tempAvgValue = 92.0f;
  telemetry.tempFlValue = 91.0f;
  telemetry.tempFrValue = 93.0f;
  telemetry.tempRlValue = 88.0f;
  telemetry.tempRrValue = 90.0f;
}

void test_begin_shows_wifi_config_when_portal_is_active() {
  FakeWifiConfigPortal wifi;
  FakeButtonInput button;
  FakeClock clock;
  FakeTelemetrySource telemetry;
  FakeStatusDisplay display;
  FakeInstrumentCluster instruments;
  App app(wifi, button, clock, telemetry, display, instruments);

  wifi.portalActive = true;

  app.begin(makeConfig());

  TEST_ASSERT_EQUAL_INT(1, button.beginCalls);
  TEST_ASSERT_EQUAL_INT(1, display.beginCalls);
  TEST_ASSERT_EQUAL_INT(1, display.showMessageCalls);
  TEST_ASSERT_EQUAL_UINT8(2, display.lastLayoutId);
  TEST_ASSERT_EQUAL_STRING("Wi-Fi SimHub", display.lastMessage1);
  TEST_ASSERT_EQUAL_STRING("Abra http://simhub", display.lastMessage3);
  TEST_ASSERT_EQUAL_INT(0, instruments.beginCalls);
  TEST_ASSERT_FALSE(telemetry.beginCalled);
}

void test_begin_starts_runtime_when_wifi_is_ready() {
  FakeWifiConfigPortal wifi;
  FakeButtonInput button;
  FakeClock clock;
  FakeTelemetrySource telemetry;
  FakeStatusDisplay display;
  FakeInstrumentCluster instruments;
  App app(wifi, button, clock, telemetry, display, instruments);

  wifi.connected = true;
  seedTelemetry(telemetry);

  app.begin(makeConfig());

  TEST_ASSERT_EQUAL_INT(1, instruments.beginCalls);
  TEST_ASSERT_TRUE(telemetry.beginCalled);
  TEST_ASSERT_EQUAL_UINT16(4242, telemetry.begunPort);
  TEST_ASSERT_EQUAL_INT(1, display.setStatusCalls);
  TEST_ASSERT_EQUAL_INT(1, display.tickCalls);
  TEST_ASSERT_TRUE(display.lastStatus.wifiConnected);
  TEST_ASSERT_TRUE(display.lastStatus.telemetryValid);
  TEST_ASSERT_EQUAL_STRING("SimHub", display.lastStatus.ssid);
  TEST_ASSERT_EQUAL_STRING("192.168.0.10", display.lastStatus.ip);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 187.5f, display.lastStatus.speedKmh);
}

void test_tick_starts_runtime_after_late_wifi_connection() {
  FakeWifiConfigPortal wifi;
  FakeButtonInput button;
  FakeClock clock;
  FakeTelemetrySource telemetry;
  FakeStatusDisplay display;
  FakeInstrumentCluster instruments;
  App app(wifi, button, clock, telemetry, display, instruments);

  wifi.portalActive = false;
  wifi.connected = false;

  app.begin(makeConfig());
  TEST_ASSERT_EQUAL_INT(1, display.showMessageCalls);
  TEST_ASSERT_FALSE(telemetry.beginCalled);

  wifi.connected = true;
  app.tick();

  TEST_ASSERT_TRUE(telemetry.beginCalled);
  TEST_ASSERT_EQUAL_INT(1, instruments.beginCalls);
  TEST_ASSERT_EQUAL_INT(2, display.beginCalls);
  TEST_ASSERT_EQUAL_INT(1, display.setStatusCalls);
}

void test_tick_shows_wifi_config_if_portal_activates_after_begin() {
  FakeWifiConfigPortal wifi;
  FakeButtonInput button;
  FakeClock clock;
  FakeTelemetrySource telemetry;
  FakeStatusDisplay display;
  FakeInstrumentCluster instruments;
  App app(wifi, button, clock, telemetry, display, instruments);

  wifi.portalActive = false;
  wifi.connected = false;

  app.begin(makeConfig());
  TEST_ASSERT_EQUAL_INT(1, display.showMessageCalls);
  TEST_ASSERT_EQUAL_STRING("Conectando Wi-Fi", display.lastMessage1);

  wifi.portalActive = true;
  app.tick();

  TEST_ASSERT_EQUAL_INT(2, display.beginCalls);
  TEST_ASSERT_EQUAL_INT(2, display.showMessageCalls);
  TEST_ASSERT_EQUAL_STRING("Wi-Fi SimHub", display.lastMessage1);
  TEST_ASSERT_FALSE(telemetry.beginCalled);
}

void test_tick_returns_to_connecting_screen_when_portal_closes() {
  FakeWifiConfigPortal wifi;
  FakeButtonInput button;
  FakeClock clock;
  FakeTelemetrySource telemetry;
  FakeStatusDisplay display;
  FakeInstrumentCluster instruments;
  App app(wifi, button, clock, telemetry, display, instruments);

  wifi.portalActive = true;
  wifi.connected = false;

  app.begin(makeConfig());
  TEST_ASSERT_EQUAL_INT(1, display.showMessageCalls);
  TEST_ASSERT_EQUAL_STRING("Wi-Fi SimHub", display.lastMessage1);

  wifi.portalActive = false;
  app.tick();

  TEST_ASSERT_EQUAL_INT(2, display.beginCalls);
  TEST_ASSERT_EQUAL_INT(2, display.showMessageCalls);
  TEST_ASSERT_EQUAL_STRING("Conectando Wi-Fi", display.lastMessage1);
  TEST_ASSERT_FALSE(telemetry.beginCalled);
}

void test_tick_updates_instruments_and_ui_during_runtime() {
  FakeWifiConfigPortal wifi;
  FakeButtonInput button;
  FakeClock clock;
  FakeTelemetrySource telemetry;
  FakeStatusDisplay display;
  FakeInstrumentCluster instruments;
  App app(wifi, button, clock, telemetry, display, instruments);

  wifi.connected = true;
  seedTelemetry(telemetry);
  clock.setMs(0);
  app.begin(makeConfig());

  clock.setMs(100);
  clock.setMicros(100000);
  app.tick();

  TEST_ASSERT_EQUAL_INT(1, telemetry.tickCalls);
  TEST_ASSERT_EQUAL_INT(1, instruments.speedCalls);
  TEST_ASSERT_EQUAL_INT(1, instruments.rpmCalls);
  TEST_ASSERT_EQUAL_INT(1, instruments.fuelCalls);
  TEST_ASSERT_EQUAL_INT(1, instruments.tireTempCalls);
  TEST_ASSERT_EQUAL_INT(1, instruments.tickCalls);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 187.5f, instruments.lastSpeed);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 6150.0f, instruments.lastRpm);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.55f, instruments.lastFuel);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 92.0f, instruments.lastTireTemp);
  TEST_ASSERT_EQUAL_INT(2, display.setStatusCalls);
  TEST_ASSERT_EQUAL_INT(2, display.tickCalls);
  TEST_ASSERT_TRUE(display.lastStatus.telemetrySignalPresent);
  TEST_ASSERT_EQUAL(TelemetryInputStatus::PacketReceived,
                    display.lastStatus.telemetryInputStatus);
  TEST_ASSERT_EQUAL_UINT32(3, display.lastStatus.telemetryDiscardedPackets);
  TEST_ASSERT_EQUAL_UINT32(1, display.lastStatus.telemetryReceiveErrors);
  TEST_ASSERT_EQUAL_INT(4, display.lastStatus.gear);
  TEST_ASSERT_EQUAL_INT(8, display.lastStatus.lapNumber);
}

void test_tick_refreshes_wifi_and_skips_optional_updates_when_telemetry_is_invalid() {
  FakeWifiConfigPortal wifi;
  FakeButtonInput button;
  FakeClock clock;
  FakeTelemetrySource telemetry;
  FakeStatusDisplay display;
  FakeInstrumentCluster instruments;
  App app(wifi, button, clock, telemetry, display, instruments);

  wifi.connected = true;
  seedTelemetry(telemetry);
  clock.setMs(0);
  app.begin(makeConfig());

  wifi.connected = false;
  telemetry.validTelemetry = false;
  telemetry.anySignal = false;
  telemetry.status = TelemetryInputStatus::NoPacket;
  telemetry.discardedPackets = 0;
  telemetry.receiveErrors = 0;
  telemetry.speed = 0.0f;
  telemetry.rpmValue = 0.0f;

  clock.setMs(500);
  clock.setMicros(500000);
  app.tick();

  TEST_ASSERT_EQUAL_INT(1, telemetry.tickCalls);
  TEST_ASSERT_EQUAL_INT(1, instruments.speedCalls);
  TEST_ASSERT_EQUAL_INT(1, instruments.rpmCalls);
  TEST_ASSERT_EQUAL_INT(0, instruments.fuelCalls);
  TEST_ASSERT_EQUAL_INT(0, instruments.tireTempCalls);
  TEST_ASSERT_EQUAL_INT(2, display.setStatusCalls);
  TEST_ASSERT_EQUAL_INT(2, display.tickCalls);
  TEST_ASSERT_FALSE(display.lastStatus.wifiConnected);
  TEST_ASSERT_EQUAL_STRING("Nao conectado", display.lastStatus.ssid);
  TEST_ASSERT_EQUAL_STRING("-", display.lastStatus.ip);
  TEST_ASSERT_FALSE(display.lastStatus.telemetryValid);
  TEST_ASSERT_FALSE(display.lastStatus.telemetrySignalPresent);
  TEST_ASSERT_EQUAL(TelemetryInputStatus::NoPacket,
                    display.lastStatus.telemetryInputStatus);
}

void test_tick_changes_layout_when_button_is_pressed() {
  FakeWifiConfigPortal wifi;
  FakeButtonInput button;
  FakeClock clock;
  FakeTelemetrySource telemetry;
  FakeStatusDisplay display;
  FakeInstrumentCluster instruments;
  App app(wifi, button, clock, telemetry, display, instruments);

  wifi.connected = true;
  seedTelemetry(telemetry);
  display.nextLayoutResult = true;

  app.begin(makeConfig());

  button.pressed = true;
  clock.setMs(10);
  clock.setMicros(10000);
  app.tick();

  TEST_ASSERT_EQUAL_INT(1, display.nextLayoutCalls);
  TEST_ASSERT_EQUAL_INT(2, display.tickCalls);
  TEST_ASSERT_EQUAL_INT(2, display.setStatusCalls);
  TEST_ASSERT_EQUAL_UINT32(10, button.lastTickMs);
}

}  // namespace

void setUp() {}
void tearDown() {}

#ifdef ARDUINO
#include <Arduino.h>
void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_begin_shows_wifi_config_when_portal_is_active);
  RUN_TEST(test_begin_starts_runtime_when_wifi_is_ready);
  RUN_TEST(test_tick_starts_runtime_after_late_wifi_connection);
  RUN_TEST(test_tick_shows_wifi_config_if_portal_activates_after_begin);
  RUN_TEST(test_tick_returns_to_connecting_screen_when_portal_closes);
  RUN_TEST(test_tick_updates_instruments_and_ui_during_runtime);
  RUN_TEST(test_tick_refreshes_wifi_and_skips_optional_updates_when_telemetry_is_invalid);
  RUN_TEST(test_tick_changes_layout_when_button_is_pressed);
  UNITY_END();
}

void loop() {}
#else
int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_begin_shows_wifi_config_when_portal_is_active);
  RUN_TEST(test_begin_starts_runtime_when_wifi_is_ready);
  RUN_TEST(test_tick_starts_runtime_after_late_wifi_connection);
  RUN_TEST(test_tick_shows_wifi_config_if_portal_activates_after_begin);
  RUN_TEST(test_tick_returns_to_connecting_screen_when_portal_closes);
  RUN_TEST(test_tick_updates_instruments_and_ui_during_runtime);
  RUN_TEST(test_tick_refreshes_wifi_and_skips_optional_updates_when_telemetry_is_invalid);
  RUN_TEST(test_tick_changes_layout_when_button_is_pressed);
  return UNITY_END();
}
#endif
