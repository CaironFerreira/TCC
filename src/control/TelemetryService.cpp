#include "TelemetryService.h"
#include <Arduino.h>

TelemetryService::TelemetryService(UdpReceiver& receiver, ITelemetryDecoder& decoder)
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
