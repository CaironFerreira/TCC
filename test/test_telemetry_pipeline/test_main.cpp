#include <stdint.h>
#include <unity.h>

#include "adapters/telemetry/Forza7Decoder.h"
#include "application/services/TelemetryService.h"
#include "support/ForzaPacketBuilder.h"
#include "support/TestDoubles.h"

using test_support::FakeClock;
using test_support::FakePacketReceiver;
using test_support::FORZA_DASH_PACKET_SIZE;
using test_support::ForzaPacketBuilder;

namespace {

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

void test_pipeline_decodes_real_forza_packet_through_service() {
  FakeClock clock;
  FakePacketReceiver receiver;
  Forza7Decoder decoder;
  TelemetryService service(receiver, decoder, clock);
  ForzaPacketBuilder packet;

  buildNominalPacket(packet);

  service.begin(5300);
  receiver.enqueuePacket(packet.data(), packet.size());
  clock.setMs(500);
  service.tick();

  TEST_ASSERT_TRUE(service.hasValidTelemetry());
  TEST_ASSERT_TRUE(service.hasAnySignal());
  TEST_ASSERT_EQUAL(TelemetryInputStatus::PacketReceived, service.inputStatus());
  TEST_ASSERT_EQUAL_UINT16(4321, service.lastFrame().rpm);
  TEST_ASSERT_EQUAL_UINT32(500, service.lastFrame().timestampMs);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 90.0f, service.speedKmh());
  TEST_ASSERT_EQUAL_INT(4, service.gear());
  TEST_ASSERT_EQUAL_INT(9, service.lapNumber());
  TEST_ASSERT_EQUAL_INT(3, service.racePosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.25f, service.fuelLevel());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 50.0f, service.tireTempFLC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 60.0f, service.tireTempFRC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 70.0f, service.tireTempRLC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 80.0f, service.tireTempRRC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 65.0f, service.tireTempAvgC());
}

void test_pipeline_preserves_last_valid_frame_when_next_packet_is_invalid() {
  FakeClock clock;
  FakePacketReceiver receiver;
  Forza7Decoder decoder;
  TelemetryService service(receiver, decoder, clock);
  ForzaPacketBuilder packet;
  uint8_t shortPacket[FORZA_DASH_PACKET_SIZE - 100] = {0};

  buildNominalPacket(packet);

  service.begin(5300);
  receiver.enqueuePacket(packet.data(), packet.size());
  clock.setMs(1000);
  service.tick();

  receiver.enqueuePacket(shortPacket, sizeof(shortPacket));
  clock.setMs(1050);
  service.tick();

  TEST_ASSERT_TRUE(service.hasValidTelemetry());
  TEST_ASSERT_EQUAL(TelemetryInputStatus::PacketReceived, service.inputStatus());
  TEST_ASSERT_EQUAL_UINT32(1000, service.lastFrame().timestampMs);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 90.0f, service.speedKmh());
}

void test_pipeline_recovers_after_timeout_with_new_valid_packet() {
  FakeClock clock;
  FakePacketReceiver receiver;
  Forza7Decoder decoder;
  TelemetryService service(receiver, decoder, clock);
  ForzaPacketBuilder firstPacket;
  ForzaPacketBuilder secondPacket;

  buildNominalPacket(firstPacket);
  buildNominalPacket(secondPacket);
  secondPacket.writeF32(244, 40.0f);
  secondPacket.writeU8(307, 5);

  service.begin(5300);
  receiver.enqueuePacket(firstPacket.data(), firstPacket.size());
  clock.setMs(2000);
  service.tick();

  clock.setMs(2301);
  service.tick();
  TEST_ASSERT_FALSE(service.hasValidTelemetry());

  receiver.enqueuePacket(secondPacket.data(), secondPacket.size());
  clock.setMs(2400);
  service.tick();

  TEST_ASSERT_TRUE(service.hasValidTelemetry());
  TEST_ASSERT_EQUAL_UINT32(2400, service.lastFrame().timestampMs);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 144.0f, service.speedKmh());
  TEST_ASSERT_EQUAL_INT(5, service.gear());
}

}  // namespace

void setUp() {}
void tearDown() {}

#ifdef ARDUINO
#include <Arduino.h>
void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_pipeline_decodes_real_forza_packet_through_service);
  RUN_TEST(test_pipeline_preserves_last_valid_frame_when_next_packet_is_invalid);
  RUN_TEST(test_pipeline_recovers_after_timeout_with_new_valid_packet);
  UNITY_END();
}

void loop() {}
#else
int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_pipeline_decodes_real_forza_packet_through_service);
  RUN_TEST(test_pipeline_preserves_last_valid_frame_when_next_packet_is_invalid);
  RUN_TEST(test_pipeline_recovers_after_timeout_with_new_valid_packet);
  return UNITY_END();
}
#endif
