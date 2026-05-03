#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <unity.h>

#include "adapters/network/udp/UdpReceiver.h"
#include "adapters/telemetry/Forza7Decoder.h"
#include "adapters/time/ArduinoClock.h"
#include "application/services/TelemetryService.h"
#include "support/ForzaPacketBuilder.h"

using test_support::FORZA_DASH_PACKET_SIZE;
using test_support::ForzaPacketBuilder;

namespace {

static const char* AP_SSID = "TCC-Test";
static const uint16_t RX_PORT = 45454;
static const uint16_t TX_PORT = 45455;
static const uint32_t HOST_ATTACH_DELAY_MS = 8000;

void buildNominalPacket(ForzaPacketBuilder& packet) {
  packet.writeF32(16, 4321.0f);
  packet.writeF32(244, 25.0f);
  packet.writeF32(256, 122.0f);
  packet.writeF32(260, 140.0f);
  packet.writeF32(264, 158.0f);
  packet.writeF32(268, 176.0f);
  packet.writeF32(276, 0.25f);
  packet.writeU16(300, 9);
  packet.writeU8(302, 3);
  packet.writeU8(307, 4);
}

bool startSoftAp() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  delay(100);
  WiFi.mode(WIFI_AP);
  delay(100);
  return WiFi.softAP(AP_SSID);
}

void stopSoftAp() {
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(100);
}

bool sendUdpPacket(const uint8_t* data, size_t size, uint16_t port) {
  WiFiUDP sender;
  if (sender.begin(TX_PORT) == 0) {
    return false;
  }

  if (!sender.beginPacket(WiFi.softAPIP(), port)) {
    sender.stop();
    return false;
  }

  const size_t written = sender.write(data, size);
  const bool ok = (written == size) && (sender.endPacket() == 1);
  sender.stop();
  return ok;
}

template <typename Predicate>
bool pumpUntil(TelemetryService& service, uint32_t timeoutMs, Predicate predicate) {
  const uint32_t start = millis();
  while ((millis() - start) < timeoutMs) {
    service.tick();
    if (predicate()) {
      return true;
    }
    delay(10);
  }
  return false;
}

void test_real_udp_pipeline_receives_valid_packet_over_softap() {
  TEST_ASSERT_TRUE(startSoftAp());
  TEST_ASSERT_NOT_EQUAL(0u, WiFi.softAPIP()[0]);

  UdpReceiver receiver;
  Forza7Decoder decoder;
  ArduinoClock clock;
  TelemetryService service(receiver, decoder, clock);
  ForzaPacketBuilder packet;

  buildNominalPacket(packet);

  TEST_ASSERT_TRUE(service.begin(RX_PORT));
  TEST_ASSERT_TRUE(sendUdpPacket(packet.data(), packet.size(), RX_PORT));

  TEST_ASSERT_TRUE(pumpUntil(service, 2000, [&service]() {
    return service.hasValidTelemetry();
  }));

  TEST_ASSERT_EQUAL(TelemetryInputStatus::PacketReceived, service.inputStatus());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 90.0f, service.speedKmh());
  TEST_ASSERT_EQUAL_INT(4, service.gear());
  TEST_ASSERT_EQUAL_INT(9, service.lapNumber());
}

void test_real_udp_pipeline_discards_oversized_packet_and_recovers() {
  TEST_ASSERT_TRUE(startSoftAp());

  UdpReceiver receiver;
  Forza7Decoder decoder;
  ArduinoClock clock;
  TelemetryService service(receiver, decoder, clock);
  ForzaPacketBuilder packet;
  uint8_t oversized[600] = {0};

  buildNominalPacket(packet);

  TEST_ASSERT_TRUE(service.begin(RX_PORT));
  TEST_ASSERT_TRUE(sendUdpPacket(oversized, sizeof(oversized), RX_PORT));
  TEST_ASSERT_TRUE(sendUdpPacket(packet.data(), packet.size(), RX_PORT));

  TEST_ASSERT_TRUE(pumpUntil(service, 2000, [&service]() {
    return service.discardedPacketCount() == 1 && service.hasValidTelemetry();
  }));

  TEST_ASSERT_EQUAL_UINT32(1, service.discardedPacketCount());
  TEST_ASSERT_EQUAL(TelemetryInputStatus::PacketReceived, service.inputStatus());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 90.0f, service.speedKmh());
}

void test_real_udp_pipeline_times_out_and_recovers_with_new_packet() {
  TEST_ASSERT_TRUE(startSoftAp());

  UdpReceiver receiver;
  Forza7Decoder decoder;
  ArduinoClock clock;
  TelemetryService service(receiver, decoder, clock);
  ForzaPacketBuilder firstPacket;
  ForzaPacketBuilder secondPacket;

  buildNominalPacket(firstPacket);
  buildNominalPacket(secondPacket);
  secondPacket.writeF32(244, 40.0f);
  secondPacket.writeU8(307, 5);

  TEST_ASSERT_TRUE(service.begin(RX_PORT));
  TEST_ASSERT_TRUE(sendUdpPacket(firstPacket.data(), firstPacket.size(), RX_PORT));
  TEST_ASSERT_TRUE(pumpUntil(service, 2000, [&service]() {
    return service.hasValidTelemetry();
  }));

  delay(350);
  service.tick();
  TEST_ASSERT_FALSE(service.hasValidTelemetry());

  TEST_ASSERT_TRUE(sendUdpPacket(secondPacket.data(), secondPacket.size(), RX_PORT));
  TEST_ASSERT_TRUE(pumpUntil(service, 2000, [&service]() {
    return service.hasValidTelemetry() && service.gear() == 5;
  }));

  TEST_ASSERT_FLOAT_WITHIN(0.001f, 144.0f, service.speedKmh());
  TEST_ASSERT_EQUAL_INT(5, service.gear());
}

}  // namespace

void setUp() {}

void tearDown() {
  stopSoftAp();
}

void setup() {
  // Give PlatformIO time to attach the serial test monitor after upload.
  delay(HOST_ATTACH_DELAY_MS);
  UNITY_BEGIN();
  RUN_TEST(test_real_udp_pipeline_receives_valid_packet_over_softap);
  RUN_TEST(test_real_udp_pipeline_discards_oversized_packet_and_recovers);
  RUN_TEST(test_real_udp_pipeline_times_out_and_recovers_with_new_packet);
  UNITY_END();
}

void loop() {}
