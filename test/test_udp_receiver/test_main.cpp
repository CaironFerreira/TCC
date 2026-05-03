#include <stdint.h>
#include <unity.h>

#include "adapters/network/udp/UdpReceiver.h"
#include "WiFiUdp.h"

namespace {

void setUpStub() {
  wifiudp_stub::reset();
}

void test_begin_forwards_port_and_result() {
  UdpReceiver receiver;

  wifiudp_stub::setBeginResult(1);
  TEST_ASSERT_TRUE(receiver.begin(7777));
  TEST_ASSERT_EQUAL_UINT32(1, wifiudp_stub::beginCalls());
  TEST_ASSERT_EQUAL_UINT16(7777, wifiudp_stub::beginPort());

  wifiudp_stub::reset();
  wifiudp_stub::setBeginResult(0);
  TEST_ASSERT_FALSE(receiver.begin(8888));
  TEST_ASSERT_EQUAL_UINT16(8888, wifiudp_stub::beginPort());
}

void test_receive_returns_no_packet_when_queue_is_empty() {
  UdpReceiver receiver;
  uint8_t buffer[16] = {0};

  PacketReceiveResult result = receiver.receive(buffer, sizeof(buffer));

  TEST_ASSERT_EQUAL(PacketReceiveStatus::NoPacket, result.status);
  TEST_ASSERT_EQUAL_UINT32(1, wifiudp_stub::parsePacketCalls());
  TEST_ASSERT_EQUAL_UINT32(0, wifiudp_stub::readBufferCalls());
}

void test_receive_reads_packet_into_buffer() {
  UdpReceiver receiver;
  uint8_t buffer[16] = {0};
  const uint8_t expected[] = {1, 2, 3, 4};

  wifiudp_stub::enqueuePacket(expected, sizeof(expected));
  PacketReceiveResult result = receiver.receive(buffer, sizeof(buffer));

  TEST_ASSERT_EQUAL(PacketReceiveStatus::PacketReceived, result.status);
  TEST_ASSERT_EQUAL_UINT32(sizeof(expected), result.bytesRead);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buffer, sizeof(expected));
  TEST_ASSERT_EQUAL_UINT32(1, wifiudp_stub::readBufferCalls());
}

void test_receive_discards_oversized_packet() {
  UdpReceiver receiver;
  uint8_t buffer[16] = {0};

  wifiudp_stub::enqueueOversizedPacket(32);
  PacketReceiveResult result = receiver.receive(buffer, sizeof(buffer));

  TEST_ASSERT_EQUAL(PacketReceiveStatus::PacketTooLarge, result.status);
  TEST_ASSERT_EQUAL_UINT32(32, wifiudp_stub::readByteCalls());
  TEST_ASSERT_EQUAL_UINT32(32, wifiudp_stub::bytesConsumed());
}

void test_receive_reports_read_error() {
  UdpReceiver receiver;
  uint8_t buffer[16] = {0};
  const uint8_t payload[] = {9, 8, 7};

  wifiudp_stub::enqueuePacketWithReadError(payload, sizeof(payload));
  PacketReceiveResult result = receiver.receive(buffer, sizeof(buffer));

  TEST_ASSERT_EQUAL(PacketReceiveStatus::ReceiveError, result.status);
  TEST_ASSERT_EQUAL_UINT32(1, wifiudp_stub::readBufferCalls());
}

}  // namespace

void setUp() { setUpStub(); }
void tearDown() {}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_begin_forwards_port_and_result);
  RUN_TEST(test_receive_returns_no_packet_when_queue_is_empty);
  RUN_TEST(test_receive_reads_packet_into_buffer);
  RUN_TEST(test_receive_discards_oversized_packet);
  RUN_TEST(test_receive_reports_read_error);
  return UNITY_END();
}
