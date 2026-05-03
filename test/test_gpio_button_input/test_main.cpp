#include <stdint.h>
#include <unity.h>

#include "adapters/input/GpioButtonInput.h"
#include "Arduino.h"

namespace {

GpioButtonInput::Config makeConfig() {
  GpioButtonInput::Config cfg;
  cfg.pin = 5;
  cfg.activeLow = true;
  cfg.useInternalPullup = true;
  cfg.useInternalPulldown = false;
  cfg.debounceMs = 40;
  return cfg;
}

void test_begin_configures_pullup_and_initial_state() {
  GpioButtonInput::Config cfg = makeConfig();
  arduino_stub::setMillis(123);
  arduino_stub::setPinLevel(cfg.pin, HIGH);
  GpioButtonInput button(cfg);

  button.begin();

  TEST_ASSERT_EQUAL_INT(INPUT_PULLUP, arduino_stub::pinModeValue(cfg.pin));
  TEST_ASSERT_EQUAL_UINT32(1, arduino_stub::pinModeCalls(cfg.pin));
  TEST_ASSERT_FALSE(button.wasPressed());
}

void test_tick_emits_press_only_after_debounce_window() {
  GpioButtonInput button(makeConfig());
  arduino_stub::setPinLevel(5, HIGH);
  arduino_stub::setMillis(0);
  button.begin();

  arduino_stub::setPinLevel(5, LOW);
  button.tick(10);
  TEST_ASSERT_FALSE(button.wasPressed());

  button.tick(30);
  TEST_ASSERT_FALSE(button.wasPressed());

  button.tick(50);
  TEST_ASSERT_TRUE(button.wasPressed());
  TEST_ASSERT_FALSE(button.wasPressed());
}

void test_tick_ignores_bounce_before_signal_stabilizes() {
  GpioButtonInput button(makeConfig());
  arduino_stub::setPinLevel(5, HIGH);
  arduino_stub::setMillis(0);
  button.begin();

  arduino_stub::setPinLevel(5, LOW);
  button.tick(10);
  arduino_stub::setPinLevel(5, HIGH);
  button.tick(20);
  arduino_stub::setPinLevel(5, LOW);
  button.tick(25);
  button.tick(50);
  TEST_ASSERT_FALSE(button.wasPressed());

  button.tick(70);
  TEST_ASSERT_TRUE(button.wasPressed());
}

void test_begin_and_tick_support_active_high_with_pulldown() {
  GpioButtonInput::Config cfg;
  cfg.pin = 7;
  cfg.activeLow = false;
  cfg.useInternalPullup = false;
  cfg.useInternalPulldown = true;
  cfg.debounceMs = 20;

  arduino_stub::setPinLevel(cfg.pin, LOW);
  arduino_stub::setMillis(0);
  GpioButtonInput button(cfg);
  button.begin();

  TEST_ASSERT_EQUAL_INT(INPUT_PULLDOWN, arduino_stub::pinModeValue(cfg.pin));

  arduino_stub::setPinLevel(cfg.pin, HIGH);
  button.tick(5);
  button.tick(25);
  TEST_ASSERT_TRUE(button.wasPressed());
}

void test_invalid_pin_is_ignored() {
  GpioButtonInput::Config cfg = makeConfig();
  cfg.pin = -1;
  GpioButtonInput button(cfg);

  button.begin();
  button.tick(100);

  TEST_ASSERT_FALSE(button.wasPressed());
  TEST_ASSERT_EQUAL_UINT32(0, arduino_stub::pinModeCalls(0));
}

}  // namespace

void setUp() { arduino_stub::reset(); }
void tearDown() {}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_begin_configures_pullup_and_initial_state);
  RUN_TEST(test_tick_emits_press_only_after_debounce_window);
  RUN_TEST(test_tick_ignores_bounce_before_signal_stabilizes);
  RUN_TEST(test_begin_and_tick_support_active_high_with_pulldown);
  RUN_TEST(test_invalid_pin_is_ignored);
  return UNITY_END();
}
