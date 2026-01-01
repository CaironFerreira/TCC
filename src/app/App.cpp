#include "App.h"
#include <Arduino.h>

App::App(TelemetryService& telemetry) : _telemetry(telemetry) {}

void App::begin() {
  _lastPrintMs = 0;
}

void App::tick() {
  uint32_t now = millis();
  if (now - _lastPrintMs < PRINT_INTERVAL_MS) return;
  _lastPrintMs = now;

  const auto& f = _telemetry.lastFrame();
}
