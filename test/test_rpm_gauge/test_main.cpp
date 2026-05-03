#include <stdint.h>
#include <unity.h>

#include "application/instruments/RpmGauge.h"
#include "support/TestDoubles.h"

using test_support::FakeClock;
using test_support::FakeGaugeMotor;

namespace {

RpmGauge::Config makeConfig() {
  RpmGauge::Config cfg;
  cfg.maxRpm = 8000.0f;
  cfg.minSteps = 0;
  cfg.maxSteps = 80;
  cfg.normalStepsPerSec = 120.0f;
  cfg.fastStepsPerSec = 320.0f;
  cfg.calibrationRiseStepsPerSec = 140.0f;
  cfg.calibrationFallStepsPerSec = 90.0f;
  return cfg;
}

void test_begin_calibrates_and_sets_zero_target() {
  FakeClock clock;
  FakeGaugeMotor motor;
  RpmGauge gauge(motor, clock, makeConfig());

  gauge.begin();

  TEST_ASSERT_EQUAL_INT(1, motor.beginCalls);
  TEST_ASSERT_EQUAL_INT(2, motor.moveCalls);
  TEST_ASSERT_EQUAL_INT(2, motor.setPositionCalls);
  TEST_ASSERT_EQUAL_INT32(0, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 120.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 120.0f, motor.lastFallStepsPerSec);
}

void test_set_rpm_uses_smoothing_for_small_changes() {
  FakeClock clock;
  FakeGaugeMotor motor;
  RpmGauge gauge(motor, clock, makeConfig());

  gauge.begin();
  motor.setCurrentPosition(0);

  gauge.setRpm(2000.0f);

  TEST_ASSERT_EQUAL_INT32(5, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 120.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 120.0f, motor.lastFallStepsPerSec);
}

void test_set_rpm_uses_fast_speed_for_large_rise() {
  FakeClock clock;
  FakeGaugeMotor motor;
  RpmGauge gauge(motor, clock, makeConfig());

  gauge.begin();
  motor.setCurrentPosition(0);

  gauge.setRpm(6000.0f);

  TEST_ASSERT_EQUAL_INT32(60, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 320.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 320.0f, motor.lastFallStepsPerSec);
}

void test_set_rpm_uses_direct_target_for_large_drop() {
  FakeClock clock;
  FakeGaugeMotor motor;
  RpmGauge gauge(motor, clock, makeConfig());

  gauge.begin();
  motor.setCurrentPosition(0);
  gauge.setRpm(6000.0f);

  motor.setCurrentPosition(60);
  gauge.setRpm(0.0f);

  TEST_ASSERT_EQUAL_INT32(0, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 320.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 320.0f, motor.lastFallStepsPerSec);
}

void test_apply_accepts_only_rpm_signal() {
  FakeClock clock;
  FakeGaugeMotor motor;
  RpmGauge gauge(motor, clock, makeConfig());

  InstrumentSignal fuelSignal;
  fuelSignal.type = InstrumentSignalType::FuelLevel;
  fuelSignal.value = 0.5f;

  InstrumentSignal rpmSignal;
  rpmSignal.type = InstrumentSignalType::Rpm;
  rpmSignal.value = 3000.0f;

  gauge.begin();
  gauge.apply(fuelSignal);
  TEST_ASSERT_EQUAL_INT32(0, motor.targetPosition());

  gauge.apply(rpmSignal);
  TEST_ASSERT_TRUE(gauge.supports(InstrumentSignalType::Rpm));
  TEST_ASSERT_FALSE(gauge.supports(InstrumentSignalType::FuelLevel));
  TEST_ASSERT_EQUAL_INT32(8, motor.targetPosition());
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
  RUN_TEST(test_set_rpm_uses_smoothing_for_small_changes);
  RUN_TEST(test_set_rpm_uses_fast_speed_for_large_rise);
  RUN_TEST(test_set_rpm_uses_direct_target_for_large_drop);
  RUN_TEST(test_apply_accepts_only_rpm_signal);
  UNITY_END();
}

void loop() {}
#else
int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_begin_calibrates_and_sets_zero_target);
  RUN_TEST(test_set_rpm_uses_smoothing_for_small_changes);
  RUN_TEST(test_set_rpm_uses_fast_speed_for_large_rise);
  RUN_TEST(test_set_rpm_uses_direct_target_for_large_drop);
  RUN_TEST(test_apply_accepts_only_rpm_signal);
  return UNITY_END();
}
#endif
