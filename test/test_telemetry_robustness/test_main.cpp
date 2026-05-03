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
  frame.speedKmh = 120.0f;
  frame.rpm = 4500;
  frame.gear = 3;
  frame.lapNumber = 4;
  frame.racePosition = 2;
  frame.fuel = 0.5f;
  frame.tireTempAvg = 140.0f;
  frame.tireTempFL = 138.0f;
  frame.tireTempFR = 140.0f;
  frame.tireTempRL = 142.0f;
  frame.tireTempRR = 144.0f;
  return frame;
}

class SequencedDecoder : public ITelemetryDecoder {
public:
  int decodeCalls = 0;

  bool decode(const uint8_t* data, size_t len, TelemetryFrame& out) override {
    (void)data;
    (void)len;
    ++decodeCalls;
    out.invalidate();
    out.valid = true;
    out.speedKmh = decodeCalls * 10.0f;
    out.rpm = (uint16_t)(1000 + decodeCalls);
    out.gear = (int8_t)((decodeCalls % 6) + 1);
    out.lapNumber = (uint16_t)decodeCalls;
    out.racePosition = (uint8_t)((decodeCalls % 12) + 1);
    out.fuel = 0.5f;
    out.tireTempAvg = 140.0f;
    out.tireTempFL = 140.0f;
    out.tireTempFR = 140.0f;
    out.tireTempRL = 140.0f;
    out.tireTempRR = 140.0f;
    return true;
  }

  const char* name() const override { return "SequencedDecoder"; }
};

class BurstPacketReceiver : public IPacketReceiver {
public:
  explicit BurstPacketReceiver(size_t packetCount) : _packetCount(packetCount) {}

  bool begin(uint16_t port) override {
    begunPort = port;
    beginCalled = true;
    return true;
  }

  PacketReceiveResult receive(uint8_t* buffer, size_t bufferSize) override {
    (void)buffer;
    (void)bufferSize;
    ++receiveCalls;

    PacketReceiveResult result;
    if (_delivered >= _packetCount) {
      result.status = PacketReceiveStatus::NoPacket;
      return result;
    }

    ++_delivered;
    result.status = PacketReceiveStatus::PacketReceived;
    result.bytesRead = 4;
    return result;
  }

  bool beginCalled = false;
  uint16_t begunPort = 0;
  size_t receiveCalls = 0;

private:
  size_t _packetCount = 0;
  size_t _delivered = 0;
};

void test_service_uses_latest_valid_frame_from_burst() {
  FakeClock clock;
  BurstPacketReceiver receiver(3);
  SequencedDecoder decoder;
  TelemetryService service(receiver, decoder, clock);

  service.begin(9100);
  clock.setMs(100);
  service.tick();

  TEST_ASSERT_TRUE(receiver.beginCalled);
  TEST_ASSERT_EQUAL_UINT16(9100, receiver.begunPort);
  TEST_ASSERT_EQUAL_INT(3, decoder.decodeCalls);
  TEST_ASSERT_TRUE(service.hasValidTelemetry());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 30.0f, service.speedKmh());
  TEST_ASSERT_EQUAL_INT(3, service.lapNumber());
}

void test_service_respects_packet_drain_limit_and_processes_remaining_packets_next_tick() {
  FakeClock clock;
  BurstPacketReceiver receiver(70);
  SequencedDecoder decoder;
  TelemetryService service(receiver, decoder, clock);

  service.begin(9200);

  clock.setMs(200);
  service.tick();

  TEST_ASSERT_EQUAL_INT(64, decoder.decodeCalls);
  TEST_ASSERT_TRUE(service.hasValidTelemetry());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 640.0f, service.speedKmh());

  clock.setMs(210);
  service.tick();

  TEST_ASSERT_EQUAL_INT(70, decoder.decodeCalls);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 700.0f, service.speedKmh());
  TEST_ASSERT_EQUAL_INT(70, service.lapNumber());
}

void test_service_survives_repeated_timeout_and_recovery_cycles() {
  FakeClock clock;
  FakePacketReceiver receiver;
  FakeTelemetryDecoder decoder;
  TelemetryService service(receiver, decoder, clock);
  uint8_t packet[4] = {0};

  decoder.frameToReturn = makeFrame();
  service.begin(9300);

  for (uint32_t cycle = 0; cycle < 10; ++cycle) {
    receiver.enqueuePacket(packet, sizeof(packet));
    clock.setMs(cycle * 1000U + 100U);
    service.tick();

    TEST_ASSERT_TRUE(service.hasValidTelemetry());
    TEST_ASSERT_TRUE(service.hasAnySignal());

    clock.setMs(cycle * 1000U + 450U);
    service.tick();

    TEST_ASSERT_FALSE(service.hasValidTelemetry());
    TEST_ASSERT_FALSE(service.hasAnySignal());
    TEST_ASSERT_EQUAL(TelemetryInputStatus::NoPacket, service.inputStatus());
  }

  TEST_ASSERT_EQUAL_UINT32(0, service.discardedPacketCount());
  TEST_ASSERT_EQUAL_UINT32(0, service.receiveErrorCount());
}

}  // namespace

void setUp() {}
void tearDown() {}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_service_uses_latest_valid_frame_from_burst);
  RUN_TEST(test_service_respects_packet_drain_limit_and_processes_remaining_packets_next_tick);
  RUN_TEST(test_service_survives_repeated_timeout_and_recovery_cycles);
  return UNITY_END();
}
