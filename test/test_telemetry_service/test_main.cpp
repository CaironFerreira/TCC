#include <math.h>
#include <stdint.h>
#include <unity.h>

#include "application/services/TelemetryService.h"
#include "support/TestDoubles.h"

using test_support::FakeClock;
using test_support::FakePacketReceiver;
using test_support::FakeTelemetryDecoder;

namespace {

TelemetryFrame makeFrame() {
  TelemetryFrame frame;
  frame.valid = true;
  frame.speedKmh = 144.0f;
  frame.rpm = 5400;
  frame.gear = 5;
  frame.lapNumber = 7;
  frame.racePosition = 2;
  frame.fuel = 1.2f;
  frame.tireTempAvg = 212.0f;
  frame.tireTempFL = 32.0f;
  frame.tireTempFR = 68.0f;
  frame.tireTempRL = 104.0f;
  frame.tireTempRR = 140.0f;
  return frame;
}

void test_begin_resets_internal_state_and_binds_port() {
  FakeClock clock;
  FakePacketReceiver receiver;
  FakeTelemetryDecoder decoder;
  TelemetryService service(receiver, decoder, clock);

  decoder.frameToReturn = makeFrame();
  uint8_t packet[8] = {0};
  receiver.enqueuePacket(packet, sizeof(packet));
  clock.setMs(10);
  service.begin(1234);
  service.tick();

  TEST_ASSERT_TRUE(service.hasValidTelemetry());
  TEST_ASSERT_TRUE(service.hasAnySignal());
  TEST_ASSERT_EQUAL(TelemetryInputStatus::PacketReceived, service.inputStatus());

  TEST_ASSERT_TRUE(service.begin(5555));
  TEST_ASSERT_TRUE(receiver.beginCalled);
  TEST_ASSERT_EQUAL_UINT16(5555, receiver.begunPort);
  TEST_ASSERT_FALSE(service.hasValidTelemetry());
  TEST_ASSERT_FALSE(service.hasAnySignal());
  TEST_ASSERT_EQUAL(TelemetryInputStatus::Idle, service.inputStatus());
  TEST_ASSERT_EQUAL_UINT32(0, service.discardedPacketCount());
  TEST_ASSERT_EQUAL_UINT32(0, service.receiveErrorCount());
}

void test_tick_updates_frame_and_derived_values() {
  FakeClock clock;
  FakePacketReceiver receiver;
  FakeTelemetryDecoder decoder;
  TelemetryService service(receiver, decoder, clock);
  uint8_t packet[16] = {0};

  decoder.frameToReturn = makeFrame();

  service.begin(9000);
  receiver.enqueuePacket(packet, sizeof(packet));
  clock.setMs(1200);
  service.tick();

  TEST_ASSERT_TRUE(service.hasValidTelemetry());
  TEST_ASSERT_TRUE(service.hasAnySignal());
  TEST_ASSERT_EQUAL(TelemetryInputStatus::PacketReceived, service.inputStatus());
  TEST_ASSERT_EQUAL_UINT32(1, decoder.decodeCalls);
  TEST_ASSERT_EQUAL_UINT32(sizeof(packet), decoder.lastLen);
  TEST_ASSERT_EQUAL_UINT32(1200, service.lastFrame().timestampMs);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 144.0f, service.speedKmh());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 5400.0f, service.rpm());
  TEST_ASSERT_EQUAL_INT(5, service.gear());
  TEST_ASSERT_EQUAL_INT(7, service.lapNumber());
  TEST_ASSERT_EQUAL_INT(2, service.racePosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, service.fuelLevel());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 100.0f, service.tireTempAvgC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.tireTempFLC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 20.0f, service.tireTempFRC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 40.0f, service.tireTempRLC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 60.0f, service.tireTempRRC());
}

void test_tick_tracks_discarded_packets_and_receive_errors() {
  FakeClock clock;
  FakePacketReceiver receiver;
  FakeTelemetryDecoder decoder;
  TelemetryService service(receiver, decoder, clock);

  service.begin(9000);

  receiver.enqueueTooLarge();
  clock.setMs(50);
  service.tick();

  TEST_ASSERT_FALSE(service.hasValidTelemetry());
  TEST_ASSERT_TRUE(service.hasAnySignal());
  TEST_ASSERT_EQUAL(TelemetryInputStatus::PacketDiscarded, service.inputStatus());
  TEST_ASSERT_EQUAL_UINT32(1, service.discardedPacketCount());
  TEST_ASSERT_EQUAL_UINT32(0, service.receiveErrorCount());

  receiver.enqueueReceiveError();
  clock.setMs(60);
  service.tick();

  TEST_ASSERT_TRUE(service.hasAnySignal());
  TEST_ASSERT_EQUAL(TelemetryInputStatus::ReceiveError, service.inputStatus());
  TEST_ASSERT_EQUAL_UINT32(1, service.discardedPacketCount());
  TEST_ASSERT_EQUAL_UINT32(1, service.receiveErrorCount());
}

void test_tick_invalidates_signal_after_timeout() {
  FakeClock clock;
  FakePacketReceiver receiver;
  FakeTelemetryDecoder decoder;
  TelemetryService service(receiver, decoder, clock);
  uint8_t packet[4] = {0};

  decoder.frameToReturn = makeFrame();

  service.begin(9000);
  receiver.enqueuePacket(packet, sizeof(packet));
  clock.setMs(1000);
  service.tick();

  TEST_ASSERT_TRUE(service.hasValidTelemetry());
  TEST_ASSERT_TRUE(service.hasAnySignal());

  clock.setMs(1301);
  service.tick();

  TEST_ASSERT_FALSE(service.hasValidTelemetry());
  TEST_ASSERT_FALSE(service.hasAnySignal());
  TEST_ASSERT_EQUAL(TelemetryInputStatus::NoPacket, service.inputStatus());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.speedKmh());
}

void test_tick_keeps_last_valid_frame_when_decoder_rejects_packet() {
  FakeClock clock;
  FakePacketReceiver receiver;
  FakeTelemetryDecoder decoder;
  TelemetryService service(receiver, decoder, clock);
  uint8_t packet[4] = {0};

  decoder.frameToReturn = makeFrame();
  service.begin(9000);
  receiver.enqueuePacket(packet, sizeof(packet));
  clock.setMs(100);
  service.tick();

  decoder.decodeResult = false;
  receiver.enqueuePacket(packet, sizeof(packet));
  clock.setMs(150);
  service.tick();

  TEST_ASSERT_TRUE(service.hasValidTelemetry());
  TEST_ASSERT_EQUAL(TelemetryInputStatus::PacketReceived, service.inputStatus());
  TEST_ASSERT_EQUAL_UINT32(100, service.lastFrame().timestampMs);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 144.0f, service.speedKmh());
}

void test_tick_invalidates_stale_frame_when_only_invalid_packets_arrive() {
  FakeClock clock;
  FakePacketReceiver receiver;
  FakeTelemetryDecoder decoder;
  TelemetryService service(receiver, decoder, clock);
  uint8_t packet[4] = {0};

  decoder.frameToReturn = makeFrame();
  service.begin(9000);
  receiver.enqueuePacket(packet, sizeof(packet));
  clock.setMs(100);
  service.tick();

  decoder.decodeResult = false;
  receiver.enqueuePacket(packet, sizeof(packet));
  clock.setMs(450);
  service.tick();

  TEST_ASSERT_FALSE(service.hasValidTelemetry());
  TEST_ASSERT_TRUE(service.hasAnySignal());
  TEST_ASSERT_EQUAL(TelemetryInputStatus::PacketReceived, service.inputStatus());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.speedKmh());
}

void test_accessors_return_zero_without_valid_frame() {
  FakeClock clock;
  FakePacketReceiver receiver;
  FakeTelemetryDecoder decoder;
  TelemetryService service(receiver, decoder, clock);

  service.begin(9000);

  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.speedKmh());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.rpm());
  TEST_ASSERT_EQUAL_INT(0, service.gear());
  TEST_ASSERT_EQUAL_INT(0, service.lapNumber());
  TEST_ASSERT_EQUAL_INT(0, service.racePosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.fuelLevel());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.tireTempAvgC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.tireTempFLC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.tireTempFRC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.tireTempRLC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.tireTempRRC());
}

void test_accessors_reject_invalid_numeric_values() {
  FakeClock clock;
  FakePacketReceiver receiver;
  FakeTelemetryDecoder decoder;
  TelemetryService service(receiver, decoder, clock);
  uint8_t packet[4] = {0};

  TelemetryFrame frame = makeFrame();
  frame.speedKmh = -10.0f;
  frame.rpm = 0;
  frame.fuel = NAN;
  frame.tireTempAvg = NAN;
  frame.tireTempFL = NAN;
  frame.tireTempFR = NAN;
  frame.tireTempRL = NAN;
  frame.tireTempRR = NAN;

  decoder.frameToReturn = frame;

  service.begin(9000);
  receiver.enqueuePacket(packet, sizeof(packet));
  clock.setMs(100);
  service.tick();

  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.speedKmh());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.rpm());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.fuelLevel());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.tireTempAvgC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.tireTempFLC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.tireTempFRC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.tireTempRLC());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.tireTempRRC());

  frame = makeFrame();
  frame.fuel = -0.25f;
  decoder.frameToReturn = frame;
  receiver.enqueuePacket(packet, sizeof(packet));
  clock.setMs(150);
  service.tick();

  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, service.fuelLevel());
}

}  // namespace

void setUp() {}
void tearDown() {}

#ifdef ARDUINO
#include <Arduino.h>
void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_begin_resets_internal_state_and_binds_port);
  RUN_TEST(test_tick_updates_frame_and_derived_values);
  RUN_TEST(test_tick_tracks_discarded_packets_and_receive_errors);
  RUN_TEST(test_tick_invalidates_signal_after_timeout);
  RUN_TEST(test_tick_keeps_last_valid_frame_when_decoder_rejects_packet);
  RUN_TEST(test_tick_invalidates_stale_frame_when_only_invalid_packets_arrive);
  RUN_TEST(test_accessors_return_zero_without_valid_frame);
  RUN_TEST(test_accessors_reject_invalid_numeric_values);
  UNITY_END();
}

void loop() {}
#else
int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_begin_resets_internal_state_and_binds_port);
  RUN_TEST(test_tick_updates_frame_and_derived_values);
  RUN_TEST(test_tick_tracks_discarded_packets_and_receive_errors);
  RUN_TEST(test_tick_invalidates_signal_after_timeout);
  RUN_TEST(test_tick_keeps_last_valid_frame_when_decoder_rejects_packet);
  RUN_TEST(test_tick_invalidates_stale_frame_when_only_invalid_packets_arrive);
  RUN_TEST(test_accessors_return_zero_without_valid_frame);
  RUN_TEST(test_accessors_reject_invalid_numeric_values);
  return UNITY_END();
}
#endif
