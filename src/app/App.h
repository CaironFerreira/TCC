#pragma once
#include "../control/TelemetryService.h"

class App {
public:
  App(TelemetryService& telemetry);

  void begin();
  void tick();

private:
  TelemetryService& _telemetry;
  uint32_t _lastPrintMs = 0;

  static constexpr uint32_t PRINT_INTERVAL_MS = 100; // 10 Hz no Serial
};
