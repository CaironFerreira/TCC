#include "application/services/TelemetryService.h"
#include <math.h>

static float fahrenheitToCelsius(float f) {
  return (f - 32.0f) * 5.0f / 9.0f;
}

TelemetryService::TelemetryService(IPacketReceiver& receiver,
                                   ITelemetryDecoder& decoder,
                                   IClock& clock)
  : _receiver(receiver), _decoder(decoder), _clock(clock) {}

bool TelemetryService::begin(uint16_t port) {
  _last.invalidate();
  _lastPacketMs = 0;
  _lastAnyPacketMs = 0;
  _hasAnySignal = false;
  _inputStatus = TelemetryInputStatus::Idle;
  _discardedPacketCount = 0;
  _receiveErrorCount = 0;
  return _receiver.begin(port);
}

void TelemetryService::tick() {
  bool receivedPacket = false;
  TelemetryInputStatus lastInputStatus = TelemetryInputStatus::NoPacket;

  for (uint8_t i = 0; i < PACKET_DRAIN_LIMIT; ++i) {
    const PacketReceiveResult result = _receiver.receive(_buf, BUF_SIZE);

    if (result.status == PacketReceiveStatus::NoPacket) {
      break;
    }

    if (result.status == PacketReceiveStatus::PacketTooLarge) {
      ++_discardedPacketCount;
      lastInputStatus = TelemetryInputStatus::PacketDiscarded;
      _lastAnyPacketMs = _clock.nowMs();
      _hasAnySignal = true;
      continue;
    }

    if (result.status == PacketReceiveStatus::ReceiveError) {
      ++_receiveErrorCount;
      lastInputStatus = TelemetryInputStatus::ReceiveError;
      _lastAnyPacketMs = _clock.nowMs();
      _hasAnySignal = true;
      break;
    }

    receivedPacket = true;
    lastInputStatus = TelemetryInputStatus::PacketReceived;
    _lastAnyPacketMs = _clock.nowMs();
    _hasAnySignal = true;

    TelemetryFrame frame;
    frame.invalidate();

    if (_decoder.decode(_buf, result.bytesRead, frame)) {
      frame.timestampMs = _clock.nowMs();
      _last = frame;
      _lastPacketMs = frame.timestampMs;
    }
  }

  _inputStatus = lastInputStatus;

  const uint32_t now = _clock.nowMs();

  // timeout de sinal (qualquer pacote)
  if (_hasAnySignal && (now - _lastAnyPacketMs) > SIGNAL_TIMEOUT_MS) {
    _hasAnySignal = false;
  }

  // timeout de frame valido, inclusive quando chegam apenas pacotes invalidos.
  if (_last.valid && (now - _lastPacketMs) > SIGNAL_TIMEOUT_MS) {
    _last.invalidate();
  }

  if (!receivedPacket) {
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
