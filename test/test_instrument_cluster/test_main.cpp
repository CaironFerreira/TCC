#include <stdint.h>
#include <unity.h>

#include "application/services/InstrumentCluster.h"
#include "support/TestDoubles.h"

using test_support::FakeInstrument;

namespace {

void test_begin_and_tick_forward_to_all_instruments() {
  FakeInstrument speed;
  FakeInstrument rpm;
  IInstrument* instruments[] = {&speed, &rpm};
  InstrumentCluster cluster(instruments, 2);

  cluster.begin();
  cluster.tick(3210);

  TEST_ASSERT_EQUAL_INT(1, speed.beginCalls);
  TEST_ASSERT_EQUAL_INT(1, rpm.beginCalls);
  TEST_ASSERT_EQUAL_INT(1, speed.tickCalls);
  TEST_ASSERT_EQUAL_INT(1, rpm.tickCalls);
  TEST_ASSERT_EQUAL_UINT32(3210, speed.lastTickMicros);
  TEST_ASSERT_EQUAL_UINT32(3210, rpm.lastTickMicros);
}

void test_dispatches_only_to_supported_instruments() {
  FakeInstrument speed;
  FakeInstrument rpm;
  FakeInstrument fuel;
  speed.supportedType = InstrumentSignalType::SpeedKmh;
  rpm.supportedType = InstrumentSignalType::Rpm;
  fuel.supportedType = InstrumentSignalType::FuelLevel;

  IInstrument* instruments[] = {&speed, &rpm, &fuel};
  InstrumentCluster cluster(instruments, 3);

  cluster.setSpeedKmh(123.0f);
  cluster.setRpm(4500.0f);
  cluster.setFuelLevel(0.75f);
  cluster.setTireTemperature(95.0f);

  TEST_ASSERT_EQUAL_INT(1, speed.applyCalls);
  TEST_ASSERT_EQUAL_INT(1, rpm.applyCalls);
  TEST_ASSERT_EQUAL_INT(1, fuel.applyCalls);
  TEST_ASSERT_EQUAL_FLOAT(123.0f, speed.receivedSignals[0].value);
  TEST_ASSERT_EQUAL_FLOAT(4500.0f, rpm.receivedSignals[0].value);
  TEST_ASSERT_EQUAL_FLOAT(0.75f, fuel.receivedSignals[0].value);
}

}  // namespace

void setUp() {}
void tearDown() {}

#ifdef ARDUINO
#include <Arduino.h>
void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_begin_and_tick_forward_to_all_instruments);
  RUN_TEST(test_dispatches_only_to_supported_instruments);
  UNITY_END();
}

void loop() {}
#else
int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_begin_and_tick_forward_to_all_instruments);
  RUN_TEST(test_dispatches_only_to_supported_instruments);
  return UNITY_END();
}
#endif
