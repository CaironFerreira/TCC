#include <stdint.h>
#include <fstream>
#include <string>
#include <vector>
#include <unity.h>

#include "adapters/telemetry/Forza7Decoder.h"
#include "application/services/TelemetryService.h"
#include "support/TestDoubles.h"

using test_support::FakeClock;
using test_support::FakePacketReceiver;

namespace {

std::vector<uint8_t> loadFixture(const char* relativePath) {
  std::ifstream input(relativePath, std::ios::binary);
  TEST_ASSERT_TRUE_MESSAGE(input.good(), relativePath);

  input.seekg(0, std::ios::end);
  const std::streamsize size = input.tellg();
  input.seekg(0, std::ios::beg);

  TEST_ASSERT_TRUE(size > 0);

  std::vector<uint8_t> bytes((size_t)size);
  input.read(reinterpret_cast<char*>(bytes.data()), size);
  TEST_ASSERT_EQUAL(size, input.gcount());
  return bytes;
}

void test_nominal_fixture_remains_backward_compatible() {
  FakeClock clock;
  FakePacketReceiver receiver;
  Forza7Decoder decoder;
  TelemetryService service(receiver, decoder, clock);
  const std::vector<uint8_t> packet = loadFixture("test/fixtures/forza_nominal_dash.bin");

  service.begin(5400);
  receiver.enqueuePacket(packet.data(), packet.size());
  clock.setMs(1000);
  service.tick();

  TEST_ASSERT_TRUE(service.hasValidTelemetry());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 90.0f, service.speedKmh());
  TEST_ASSERT_EQUAL_INT(4, service.gear());
  TEST_ASSERT_EQUAL_INT(9, service.lapNumber());
  TEST_ASSERT_EQUAL_INT(3, service.racePosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.25f, service.fuelLevel());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 65.0f, service.tireTempAvgC());
}

void test_reverse_fixture_preserves_gear_mapping_and_clamps_fuel() {
  FakeClock clock;
  FakePacketReceiver receiver;
  Forza7Decoder decoder;
  TelemetryService service(receiver, decoder, clock);
  const std::vector<uint8_t> packet = loadFixture("test/fixtures/forza_reverse_dash.bin");

  service.begin(5401);
  receiver.enqueuePacket(packet.data(), packet.size());
  clock.setMs(1100);
  service.tick();

  TEST_ASSERT_TRUE(service.hasValidTelemetry());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 9.0f, service.speedKmh());
  TEST_ASSERT_EQUAL_INT(-1, service.gear());
  TEST_ASSERT_EQUAL_INT(12, service.lapNumber());
  TEST_ASSERT_EQUAL_INT(8, service.racePosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, service.fuelLevel());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 35.0f, service.tireTempAvgC());
}

void test_highspeed_fixture_preserves_hotlap_readings() {
  FakeClock clock;
  FakePacketReceiver receiver;
  Forza7Decoder decoder;
  TelemetryService service(receiver, decoder, clock);
  const std::vector<uint8_t> packet = loadFixture("test/fixtures/forza_highspeed_dash.bin");

  service.begin(5402);
  receiver.enqueuePacket(packet.data(), packet.size());
  clock.setMs(1200);
  service.tick();

  TEST_ASSERT_TRUE(service.hasValidTelemetry());
  TEST_ASSERT_FLOAT_WITHIN(0.01f, 275.04f, service.speedKmh());
  TEST_ASSERT_EQUAL_INT(6, service.gear());
  TEST_ASSERT_EQUAL_INT(27, service.lapNumber());
  TEST_ASSERT_EQUAL_INT(1, service.racePosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.05f, service.fuelLevel());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 99.583f, service.tireTempAvgC());
}

}  // namespace

void setUp() {}
void tearDown() {}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_nominal_fixture_remains_backward_compatible);
  RUN_TEST(test_reverse_fixture_preserves_gear_mapping_and_clamps_fuel);
  RUN_TEST(test_highspeed_fixture_preserves_hotlap_readings);
  return UNITY_END();
}
