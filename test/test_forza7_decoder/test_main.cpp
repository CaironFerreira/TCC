#include <math.h>
#include <stdint.h>
#include <unity.h>

#include "adapters/telemetry/Forza7Decoder.h"
#include "domain/TelemetryFrame.h"
#include "support/ForzaPacketBuilder.h"

using test_support::FORZA_DASH_PACKET_SIZE;
using test_support::ForzaPacketBuilder;

namespace {

void test_decode_rejects_short_packet() {
  Forza7Decoder decoder;
  TelemetryFrame frame;
  frame.valid = true;
  frame.speedKmh = 123.0f;

  TEST_ASSERT_FALSE(decoder.decode(nullptr, 0, frame));
  TEST_ASSERT_FALSE(frame.valid);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, frame.speedKmh);

  uint8_t shortPacket[FORZA_DASH_PACKET_SIZE - 1] = {0};
  frame.valid = true;
  frame.rpm = 777;

  TEST_ASSERT_FALSE(decoder.decode(shortPacket, sizeof(shortPacket), frame));
  TEST_ASSERT_FALSE(frame.valid);
  TEST_ASSERT_EQUAL_UINT16(0, frame.rpm);
}

void test_decode_maps_known_packet_fields() {
  Forza7Decoder decoder;
  ForzaPacketBuilder packet;
  TelemetryFrame frame;

  packet.writeF32(16, 4321.0f);
  packet.writeF32(244, 50.0f);
  packet.writeF32(256, 212.0f);
  packet.writeF32(260, 194.0f);
  packet.writeF32(264, 176.0f);
  packet.writeF32(268, 158.0f);
  packet.writeF32(276, 0.65f);
  packet.writeU16(300, 12);
  packet.writeU8(302, 4);
  packet.writeU8(307, 3);

  TEST_ASSERT_TRUE(decoder.decode(packet.data(), packet.size(), frame));
  TEST_ASSERT_TRUE(frame.valid);
  TEST_ASSERT_EQUAL_UINT16(4321, frame.rpm);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 180.0f, frame.speedKmh);
  TEST_ASSERT_EQUAL_INT8(3, frame.gear);
  TEST_ASSERT_EQUAL_UINT16(12, frame.lapNumber);
  TEST_ASSERT_EQUAL_UINT8(4, frame.racePosition);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.65f, frame.fuel);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 212.0f, frame.tireTempFL);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 194.0f, frame.tireTempFR);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 176.0f, frame.tireTempRL);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 158.0f, frame.tireTempRR);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 185.0f, frame.tireTempAvg);
}

void test_decode_maps_zero_gear_to_reverse() {
  Forza7Decoder decoder;
  ForzaPacketBuilder packet;
  TelemetryFrame frame;

  packet.writeF32(16, 1000.0f);
  packet.writeF32(244, 0.0f);
  packet.writeU8(307, 0);

  TEST_ASSERT_TRUE(decoder.decode(packet.data(), packet.size(), frame));
  TEST_ASSERT_EQUAL_INT8(-1, frame.gear);
}

}  // namespace

void setUp() {}
void tearDown() {}

#ifdef ARDUINO
#include <Arduino.h>
void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_decode_rejects_short_packet);
  RUN_TEST(test_decode_maps_known_packet_fields);
  RUN_TEST(test_decode_maps_zero_gear_to_reverse);
  UNITY_END();
}

void loop() {}
#else
int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_decode_rejects_short_packet);
  RUN_TEST(test_decode_maps_known_packet_fields);
  RUN_TEST(test_decode_maps_zero_gear_to_reverse);
  return UNITY_END();
}
#endif
