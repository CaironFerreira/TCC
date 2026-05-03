#include <stdint.h>
#include <unity.h>

#include "adapters/network/wifi/WiFiConfigPortal.h"
#include "domain/UiStatus.h"
#include "support/stubs/Arduino.h"
#include "support/stubs/DNSServer.h"
#include "support/stubs/Preferences.h"
#include "support/stubs/WebServer.h"
#include "support/stubs/WiFi.h"

namespace {

WiFiConfigPortal::Config makeConfig() {
  WiFiConfigPortal::Config cfg;
  cfg.apSsid = "SimHub-Test";
  cfg.apPassword = nullptr;
  cfg.connectTimeoutMs = 500;
  return cfg;
}

void test_begin_without_saved_credentials_starts_configuration_portal() {
  WiFiConfigPortal portal(makeConfig());
  char ip[UiStatus::IP_CAPACITY] = "";

  portal.begin();
  portal.copyLocalIp(ip, sizeof(ip));

  TEST_ASSERT_TRUE(portal.isPortalActive());
  TEST_ASSERT_FALSE(portal.isConnected());
  TEST_ASSERT_EQUAL_STRING("192.168.4.1", ip);
  TEST_ASSERT_EQUAL_UINT32(1, dnsserver_stub::state().startCalls);
  TEST_ASSERT_EQUAL_INT(WIFI_AP, wifi_stub::state().mode);
}

void test_saved_credentials_timeout_reopens_portal_asynchronously() {
  preferences_stub::setString("simhub_wifi", "ssid", "MinhaRede");
  preferences_stub::setString("simhub_wifi", "password", "12345678");
  wifi_stub::setStatus(WL_IDLE_STATUS);

  WiFiConfigPortal portal(makeConfig());

  portal.begin();
  TEST_ASSERT_FALSE(portal.isPortalActive());
  TEST_ASSERT_EQUAL_STRING("connecting", portal.connectionState().c_str());
  TEST_ASSERT_EQUAL_UINT32(1, wifi_stub::state().beginCalls);

  arduino_stub::setMillis(1000);
  portal.tick();

  TEST_ASSERT_TRUE(portal.isPortalActive());
  TEST_ASSERT_EQUAL_STRING("error", portal.connectionState().c_str());
  TEST_ASSERT_EQUAL_STRING("Falha na conexão.", portal.lastStatusMessage().c_str());
}

void test_saved_credentials_complete_connection_without_blocking_begin() {
  preferences_stub::setString("simhub_wifi", "ssid", "MinhaRede");
  preferences_stub::setString("simhub_wifi", "password", "12345678");
  wifi_stub::setStatus(WL_IDLE_STATUS);

  WiFiConfigPortal portal(makeConfig());
  char ssid[UiStatus::SSID_CAPACITY] = "";
  char ip[UiStatus::IP_CAPACITY] = "";

  portal.begin();
  wifi_stub::setStatus(WL_CONNECTED);
  wifi_stub::setConnectedSsid("MinhaRede");
  wifi_stub::setLocalIp(192, 168, 0, 77);

  portal.tick();
  portal.copyConnectedSsid(ssid, sizeof(ssid));
  portal.copyLocalIp(ip, sizeof(ip));

  TEST_ASSERT_TRUE(portal.isConnected());
  TEST_ASSERT_FALSE(portal.isPortalActive());
  TEST_ASSERT_EQUAL_STRING("success", portal.connectionState().c_str());
  TEST_ASSERT_EQUAL_STRING("MinhaRede", ssid);
  TEST_ASSERT_EQUAL_STRING("192.168.0.77", ip);
}

}  // namespace

void setUp() {
  arduino_stub::reset();
  wifi_stub::reset();
  preferences_stub::reset();
  dnsserver_stub::reset();
  webserver_stub::reset();
}

void tearDown() {}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_begin_without_saved_credentials_starts_configuration_portal);
  RUN_TEST(test_saved_credentials_timeout_reopens_portal_asynchronously);
  RUN_TEST(test_saved_credentials_complete_connection_without_blocking_begin);
  return UNITY_END();
}
