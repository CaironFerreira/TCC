#include <stdint.h>
#include <unity.h>

#include "adapters/display/DisplayService.h"
#include "adapters/display/IDisplayLayout.h"
#include "support/stubs/Arduino.h"
#include "support/stubs/TFT_eSPI.h"

namespace {

class FakeLayout : public IDisplayLayout {
public:
  int beginCalls = 0;
  int renderCalls = 0;
  bool lastForceAll = false;
  unsigned long lastNow = 0;
  UiStatus lastStatus{};

  void begin(TFT_eSPI& display) override {
    ++beginCalls;
    lastDisplay = &display;
  }

  void render(const UiStatus& status, bool forceAll, unsigned long now) override {
    ++renderCalls;
    lastStatus = status;
    lastForceAll = forceAll;
    lastNow = now;
  }

  TFT_eSPI* lastDisplay = nullptr;
};

void test_begin_falls_back_to_first_layout_and_tick_renders_with_throttle() {
  FakeLayout firstLayout;
  FakeLayout secondLayout;
  DisplayLayoutEntry layouts[] = {
    {1, &firstLayout},
    {2, &secondLayout},
  };
  DisplayService service(layouts, sizeof(layouts) / sizeof(layouts[0]));
  UiStatus status;

  status.telemetrySignalPresent = true;
  status.telemetryInputStatus = TelemetryInputStatus::PacketReceived;
  status.telemetryValid = true;
  status.speedKmh = 88.0f;

  TEST_ASSERT_TRUE(service.begin(99));
  TEST_ASSERT_EQUAL_UINT8(1, service.currentLayoutId());
  TEST_ASSERT_EQUAL_INT(1, firstLayout.beginCalls);
  TEST_ASSERT_EQUAL_INT(0, secondLayout.beginCalls);
  TEST_ASSERT_EQUAL_UINT32(1, tft_stub::state().initCalls);

  service.setStatus(status);
  service.tick();

  TEST_ASSERT_EQUAL_INT(1, firstLayout.renderCalls);
  TEST_ASSERT_TRUE(firstLayout.lastForceAll);
  TEST_ASSERT_FLOAT_WITHIN(0.001f, 88.0f, firstLayout.lastStatus.speedKmh);

  arduino_stub::setMillis(50);
  service.tick();
  TEST_ASSERT_EQUAL_INT(1, firstLayout.renderCalls);

  arduino_stub::setMillis(100);
  service.tick();
  TEST_ASSERT_EQUAL_INT(2, firstLayout.renderCalls);
  TEST_ASSERT_FALSE(firstLayout.lastForceAll);
  TEST_ASSERT_EQUAL_UINT32(100, firstLayout.lastNow);
}

void test_next_layout_cycles_and_show_message_draws_text() {
  FakeLayout firstLayout;
  FakeLayout secondLayout;
  DisplayLayoutEntry layouts[] = {
    {1, &firstLayout},
    {2, &secondLayout},
  };
  DisplayService service(layouts, sizeof(layouts) / sizeof(layouts[0]));

  TEST_ASSERT_TRUE(service.begin(1));
  TEST_ASSERT_TRUE(service.nextLayout());
  TEST_ASSERT_EQUAL_UINT8(2, service.currentLayoutId());
  TEST_ASSERT_EQUAL_INT(1, secondLayout.beginCalls);

  service.showMessage("Linha 1", "Linha 2", "Linha 3", "Linha 4");

  TEST_ASSERT_EQUAL_UINT32(1, tft_stub::state().fillScreenCalls);
  TEST_ASSERT_EQUAL_UINT32(4, tft_stub::state().drawCentreStringCalls);
  TEST_ASSERT_EQUAL_STRING("Linha 4", tft_stub::state().lastDrawnText.c_str());
}

}  // namespace

void setUp() {
  arduino_stub::reset();
  tft_stub::reset();
}

void tearDown() {}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_begin_falls_back_to_first_layout_and_tick_renders_with_throttle);
  RUN_TEST(test_next_layout_cycles_and_show_message_draws_text);
  return UNITY_END();
}
