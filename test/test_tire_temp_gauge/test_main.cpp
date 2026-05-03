#include <stdint.h>
#include <unity.h>

#include "application/instruments/TireTempGauge.h"
#include "support/TestDoubles.h"

using test_support::FakeClock;
using test_support::FakeGaugeMotor;

namespace {

TireTempGauge::Config makeConfig() {
  TireTempGauge::Config cfg;
  cfg.minTemp = 20.0f;
  cfg.maxTemp = 120.0f;
  cfg.minSteps = 0;
  cfg.maxSteps = 100;
  cfg.normalStepsPerSec = 150.0f;
  cfg.fastStepsPerSec = 240.0f;
  cfg.calibrationBackoffSteps = 8;
  cfg.calibrationRiseStepsPerSec = 140.0f;
  cfg.calibrationFallStepsPerSec = 100.0f;
  return cfg;
}

void test_begin_calibrates_with_backoff_and_zero_target() {
  FakeClock clock;
  FakeGaugeMotor motor;
  TireTempGauge gauge(motor, clock, makeConfig());

  gauge.begin();

  TEST_ASSERT_EQUAL_INT(1, motor.beginCalls);
  TEST_ASSERT_EQUAL_INT(3, motor.moveCalls);
  TEST_ASSERT_EQUAL_INT(3, motor.setPositionCalls);
  TEST_ASSERT_EQUAL_INT32(0, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 150.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 150.0f, motor.lastFallStepsPerSec);
}

void test_first_temperature_uses_direct_target() {
  FakeClock clock;
  FakeGaugeMotor motor;
  TireTempGauge gauge(motor, clock, makeConfig());

  gauge.begin();
  gauge.setTemperature(70.0f);

  TEST_ASSERT_EQUAL_INT32(50, motor.targetPosition());
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 150.0f, motor.lastRiseStepsPerSec);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 240.0f, motor.lastFallStepsPerSec);
}

void test_subsequent_temperature_uses_smoothing() {
  FakeClock clock;
  FakeGaugeMotor motor;
  TireTempGauge gauge(motor, clock, makeConfig());

  gauge.begin();
  gauge.setTemperature(70.0f);
  motor.setCurrentPosition(50);

  gauge.setTemperature(80.0f);

  TEST_ASSERT_EQUAL_INT32(52, motor.targetPosition());
}

void test_inverted_direction_reverses_target_steps() {
  FakeClock clock;
  FakeGaugeMotor motor;
  TireTempGauge::Config cfg = makeConfig();
  cfg.minTemp = 0.0f;
  cfg.maxTemp = 100.0f;
  cfg.invertIndicationDirection = true;
  TireTempGauge gauge(motor, clock, cfg);

  gauge.begin();
  gauge.setTemperature(75.0f);

  TEST_ASSERT_EQUAL_INT32(-75, motor.targetPosition());
}

void test_apply_clamps_invalid_values_and_accepts_only_temperature_signal() {
  FakeClock clock;
  FakeGaugeMotor motor;
  TireTempGauge gauge(motor, clock, makeConfig());

  InstrumentSignal speedSignal;
  speedSignal.type = InstrumentSignalType::SpeedKmh;
  speedSignal.value = 200.0f;

  InstrumentSignal tempSignal;
  tempSignal.type = InstrumentSignalType::TireTemperatureC;
  tempSignal.value = 150.0f;

  gauge.begin();
  gauge.apply(speedSignal);
  TEST_ASSERT_EQUAL_INT32(0, motor.targetPosition());

  gauge.apply(tempSignal);
  TEST_ASSERT_TRUE(gauge.supports(InstrumentSignalType::TireTemperatureC));
  TEST_ASSERT_FALSE(gauge.supports(InstrumentSignalType::SpeedKmh));
  TEST_ASSERT_EQUAL_INT32(100, motor.targetPosition());
}

}  // namespace

void setUp() {}
void tearDown() {}

#ifdef ARDUINO
#include <Arduino.h>
void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(test_begin_calibrates_with_backoff_and_zero_target);
  RUN_TEST(test_first_temperature_uses_direct_target);
  RUN_TEST(test_subsequent_temperature_uses_smoothing);
  RUN_TEST(test_inverted_direction_reverses_target_steps);
  RUN_TEST(test_apply_clamps_invalid_values_and_accepts_only_temperature_signal);
  UNITY_END();
}

void loop() {}
#else
int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_begin_calibrates_with_backoff_and_zero_target);
  RUN_TEST(test_first_temperature_uses_direct_target);
  RUN_TEST(test_subsequent_temperature_uses_smoothing);
  RUN_TEST(test_inverted_direction_reverses_target_steps);
  RUN_TEST(test_apply_clamps_invalid_values_and_accepts_only_temperature_signal);
  return UNITY_END();
}
#endif
