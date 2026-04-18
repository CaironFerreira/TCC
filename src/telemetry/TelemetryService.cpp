#include "telemetry/TelemetryService.h"
#include <Arduino.h>
#include <math.h>

static float fahrenheitToCelsius(float f) {
  return (f - 32.0f) * 5.0f / 9.0f;
}

TelemetryService::TelemetryService(IPacketReceiver& receiver, ITelemetryDecoder& decoder)
  : _receiver(receiver), _decoder(decoder) {}

bool TelemetryService::begin(uint16_t port) {
  _last.invalidate();
  _lastPacketMs = 0;
  _lastAnyPacketMs = 0;
  _hasAnySignal = false;
  return _receiver.begin(port);
}

void TelemetryService::tick() {
  int n = _receiver.receive(_buf, BUF_SIZE);
  if (n <= 0) {
    // timeout de sinal (qualquer pacote)
    if (_hasAnySignal && (millis() - _lastAnyPacketMs) > SIGNAL_TIMEOUT_MS) {
      _hasAnySignal = false;
    }

    // timeout de frame valido
    if (_last.valid && (millis() - _lastPacketMs) > SIGNAL_TIMEOUT_MS) {
      _last.invalidate();
    }
    return;
  }

  _lastAnyPacketMs = millis();
  _hasAnySignal = true;

  TelemetryFrame frame;
  frame.invalidate();

  if (_decoder.decode(_buf, (size_t)n, frame)) {
    _last = frame;
    _lastPacketMs = millis();
  }
}

bool TelemetryService::hasValidTelemetry() const {
  return _last.valid;
}

float TelemetryService::speedKmh() const {
  if (!_last.valid) {
    return 0.0f;
  }

  if (!isfinite(_last.speedKmh) || _last.speedKmh <= 0.0f) {
    return 0.0f;
  }

  return _last.speedKmh;
}

float TelemetryService::rpm() const {
  if (!_last.valid) {
    return 0.0f;
  }

  if (!isfinite(_last.rpm) || _last.rpm <= 0.0f) {
    return 0.0f;
  }

  return _last.rpm;
}

int TelemetryService::gear() const {
  if (!_last.valid) {
    return 0;
  }

  return _last.gear;
}

float TelemetryService::fuelLevel() const {
  if (!_last.valid) {
    return 0.0f;
  }

  if (!isfinite(_last.fuel) || _last.fuel < 0.0f) {
    return 0.0f;
  }

  return _last.fuel;
}

float TelemetryService::tireTempAvgC() const {
  if (!_last.valid) {
    return 0.0f;
  }

  if (!isfinite(_last.tireTempAvg)) {
    return 0.0f;
  }

  return fahrenheitToCelsius(_last.tireTempAvg);
}
