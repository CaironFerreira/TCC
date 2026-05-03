#include <stdint.h>
#include <unity.h>

#include "application/instruments/FuelGauge.h"
#include "support/TestDoubles.h"

using test_support::FakeClock;
using test_support::FakeGaugeMotor;

namespace {

FuelGauge::Config makeConfig() {
  FuelGauge::Config cfg;
  cfg.minFuelLevel = 0.0f;
  cfg.maxFuelLevel = 1.0f;
  cfg.minSteps = 0;
  cfg.maxSteps = 100;
  cfg.normalStepsPerSec = 90.0f;
  cfg.fastStepsPerSec = 240.0f;
  cfg.calibrationRiseStepsPerSec = 140.0f;
  cfg.calibrationFallStepsPerSec = 100.0f;
  return cfg;
}

void test_begin_calibrates_and_sets_zero_target() {
  FakeClock clock;
  FakeGaugeMotor motor;
  FuelGauge gauge(motor, clock, makeConfig());

  gauge.begin();

  TEST_ASSERT_EQUAL_INT(1, motor.beginCalls);
  TEST_ASSERT_EQUAL_INT(2, motor.moveCalls);
  TEST_ASSERT_EQUAL_INT(2, motor.setPositionCalls);
  TEST_ASSERT_EQUAL_INT32(0, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 90.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 90.0f, motor.lastFallStepsPerSec);
}

void test_first_fuel_level_uses_direct_target() {
  FakeClock clock;
  FakeGaugeMotor motor;
  FuelGauge gauge(motor, clock, makeConfig());

  gauge.begin();
  gauge.setFuelLevel(0.4f);

  TEST_ASSERT_EQUAL_INT32(40, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 240.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 240.0f, motor.lastFallStepsPerSec);
}

void test_subsequent_small_change_uses_smoothing() {
  FakeClock clock;
  FakeGaugeMotor motor;
  FuelGauge gauge(motor, clock, makeConfig());

  gauge.begin();
  gauge.setFuelLevel(0.4f);
  motor.setCurrentPosition(40);

  gauge.setFuelLevel(0.5f);

  TEST_ASSERT_EQUAL_INT32(43, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 90.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 90.0f, motor.lastFallStepsPerSec);
}

void test_large_drop_uses_direct_target_and_fast_speed() {
  FakeClock clock;
  FakeGaugeMotor motor;
  FuelGauge gauge(motor, clock, makeConfig());

  gauge.begin();
  gauge.setFuelLevel(0.8f);
  motor.setCurrentPosition(80);

  gauge.setFuelLevel(0.0f);

  TEST_ASSERT_EQUAL_INT32(0, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 240.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 240.0f, motor.lastFallStepsPerSec);
}

void test_clamps_invalid_and_out_of_range_values() {
  FakeClock clock;
  FakeGaugeMotor motor;
  FuelGauge gauge(motor, clock, makeConfig());

  gauge.begin();
  gauge.setFuelLevel(2.0f);
  TEST_ASSERT_EQUAL_INT32(100, motor.targetPosition());

  motor.setCurrentPosition(100);
  gauge.setFuelLevel(-1.0f);
  TEST_ASSERT_EQUAL_INT32(0, motor.targetPosition());
}

}  // namespace

void setUp() {}
void tearDown() {}

#ifdef ARDUINO
#include <Arduino.h>
void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_begin_calibrates_and_sets_zero_target);
  RUN_TEST(test_first_fuel_level_uses_direct_target);
  RUN_TEST(test_subsequent_small_change_uses_smoothing);
  RUN_TEST(test_large_drop_uses_direct_target_and_fast_speed);
  RUN_TEST(test_clamps_invalid_and_out_of_range_values);
  UNITY_END();
}

void loop() {}
#else
int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_begin_calibrates_and_sets_zero_target);
  RUN_TEST(test_first_fuel_level_uses_direct_target);
  RUN_TEST(test_subsequent_small_change_uses_smoothing);
  RUN_TEST(test_large_drop_uses_direct_target_and_fast_speed);
  RUN_TEST(test_clamps_invalid_and_out_of_range_values);
  return UNITY_END();
}
#endif
