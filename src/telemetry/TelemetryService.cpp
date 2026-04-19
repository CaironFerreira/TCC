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
  bool receivedPacket = false;

  for (uint8_t i = 0; i < PACKET_DRAIN_LIMIT; ++i) {
    int n = _receiver.receive(_buf, BUF_SIZE);
    if (n <= 0) {
      break;
    }

    receivedPacket = true;
    _lastAnyPacketMs = millis();
    _hasAnySignal = true;

    TelemetryFrame frame;
    frame.invalidate();

    if (_decoder.decode(_buf, (size_t)n, frame)) {
      _last = frame;
      _lastPacketMs = millis();
    }
  }

  if (!receivedPacket) {
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

int TelemetryService::lapNumber() const {
  if (!_last.valid) {
    return 0;
  }

  return _last.lapNumber;
}

int TelemetryService::racePosition() const {
  if (!_last.valid) {
    return 0;
  }

  return _last.racePosition;
}

float TelemetryService::fuelLevel() const {
  if (!_last.valid) {
    return 0.0f;
  }

  if (!isfinite(_last.fuel)) {
    return 0.0f;
  }

  float fuelRemaining = _last.fuel;
  if (fuelRemaining < 0.0f) {
    fuelRemaining = 0.0f;
  } else if (fuelRemaining > 1.0f) {
    fuelRemaining = 1.0f;
  }

  return fuelRemaining;
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

float TelemetryService::tireTempFLC() const {
  if (!_last.valid || !isfinite(_last.tireTempFL)) {
    return 0.0f;
  }

  return fahrenheitToCelsius(_last.tireTempFL);
}

float TelemetryService::tireTempFRC() const {
  if (!_last.valid || !isfinite(_last.tireTempFR)) {
    return 0.0f;
  }

  return fahrenheitToCelsius(_last.tireTempFR);
}

float TelemetryService::tireTempRLC() const {
  if (!_last.valid || !isfinite(_last.tireTempRL)) {
    return 0.0f;
  }

  return fahrenheitToCelsius(_last.tireTempRL);
}

float TelemetryService::tireTempRRC() const {
  if (!_last.valid || !isfinite(_last.tireTempRR)) {
    return 0.0f;
  }

  return fahrenheitToCelsius(_last.tireTempRR);
}
