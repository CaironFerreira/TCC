#include <stdint.h>
#include <unity.h>

#include "application/instruments/SpeedGauge.h"
#include "support/TestDoubles.h"

using test_support::FakeClock;
using test_support::FakeGaugeMotor;

namespace {

SpeedGauge::Config makeConfig() {
  SpeedGauge::Config cfg;
  cfg.maxSpeedKmh = 200.0f;
  cfg.minSteps = 0;
  cfg.maxSteps = 100;
  cfg.normalStepsPerSec = 50.0f;
  cfg.fastStepsPerSec = 300.0f;
  cfg.calibrationRiseStepsPerSec = 120.0f;
  cfg.calibrationFallStepsPerSec = 90.0f;
  return cfg;
}

void test_begin_calibrates_and_sets_zero_target() {
  FakeClock clock;
  FakeGaugeMotor motor;
  SpeedGauge gauge(motor, clock, makeConfig());

  gauge.begin();

  TEST_ASSERT_EQUAL_INT(1, motor.beginCalls);
  TEST_ASSERT_EQUAL_INT(3, motor.moveCalls);
  TEST_ASSERT_EQUAL_INT(3, motor.setPositionCalls);
  TEST_ASSERT_EQUAL_INT32(0, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 50.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 50.0f, motor.lastFallStepsPerSec);
}

void test_set_speed_uses_smoothing_for_small_changes() {
  FakeClock clock;
  FakeGaugeMotor motor;
  SpeedGauge gauge(motor, clock, makeConfig());

  gauge.begin();
  motor.setCurrentPosition(0);

  gauge.setSpeedKmh(20.0f);

  TEST_ASSERT_EQUAL_INT32(3, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 50.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 50.0f, motor.lastFallStepsPerSec);
}

void test_set_speed_uses_fast_speed_for_large_rise() {
  FakeClock clock;
  FakeGaugeMotor motor;
  SpeedGauge gauge(motor, clock, makeConfig());

  gauge.begin();
  motor.setCurrentPosition(0);

  gauge.setSpeedKmh(100.0f);

  TEST_ASSERT_EQUAL_INT32(13, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 300.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 300.0f, motor.lastFallStepsPerSec);
}

void test_set_speed_uses_direct_target_for_large_drop() {
  FakeClock clock;
  FakeGaugeMotor motor;
  SpeedGauge gauge(motor, clock, makeConfig());

  gauge.begin();
  motor.setCurrentPosition(0);
  gauge.setSpeedKmh(80.0f);

  motor.setCurrentPosition(60);
  gauge.setSpeedKmh(0.0f);

  TEST_ASSERT_EQUAL_INT32(0, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 300.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 300.0f, motor.lastFallStepsPerSec);
}

void test_apply_accepts_only_speed_signal() {
  FakeClock clock;
  FakeGaugeMotor motor;
  SpeedGauge gauge(motor, clock, makeConfig());

  InstrumentSignal rpmSignal;
  rpmSignal.type = InstrumentSignalType::Rpm;
  rpmSignal.value = 5000.0f;

  InstrumentSignal speedSignal;
  speedSignal.type = InstrumentSignalType::SpeedKmh;
  speedSignal.value = 40.0f;

  gauge.begin();
  gauge.apply(rpmSignal);
  TEST_ASSERT_EQUAL_INT32(0, motor.targetPosition());

  gauge.apply(speedSignal);
  TEST_ASSERT_TRUE(gauge.supports(InstrumentSignalType::SpeedKmh));
  TEST_ASSERT_FALSE(gauge.supports(InstrumentSignalType::Rpm));
  TEST_ASSERT_EQUAL_INT32(5, motor.targetPosition());
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
  RUN_TEST(test_set_speed_uses_smoothing_for_small_changes);
  RUN_TEST(test_set_speed_uses_fast_speed_for_large_rise);
  RUN_TEST(test_set_speed_uses_direct_target_for_large_drop);
  RUN_TEST(test_apply_accepts_only_speed_signal);
  UNITY_END();
}

void loop() {}
#else
int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_begin_calibrates_and_sets_zero_target);
  RUN_TEST(test_set_speed_uses_smoothing_for_small_changes);
  RUN_TEST(test_set_speed_uses_fast_speed_for_large_rise);
  RUN_TEST(test_set_speed_uses_direct_target_for_large_drop);
  RUN_TEST(test_apply_accepts_only_speed_signal);
  return UNITY_END();
}
#endif
