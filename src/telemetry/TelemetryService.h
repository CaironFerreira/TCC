#pragma once
#include <stddef.h>
#include <stdint.h>
#include "domain/TelemetryFrame.h"
#include "drivers/network/udp/UdpReceiver.h"
#include "telemetry/decoders/ITelemetryDecoder.h"

class TelemetryService {
public:
  TelemetryService(UdpReceiver& receiver, ITelemetryDecoder& decoder);

  bool begin(uint16_t port);
  void tick();

  const TelemetryFrame& lastFrame() const { return _last; }

  bool hasValidTelemetry() const;
  bool hasAnySignal() const { return _hasAnySignal; }

  float speedKmh() const;
  float rpm() const;
  int gear() const;
  float fuelLevel() const;
  float tireTempAvgC() const;

private:
  UdpReceiver& _receiver;
  ITelemetryDecoder& _decoder;

  TelemetryFrame _last;
  uint32_t _lastPacketMs = 0;
  uint32_t _lastAnyPacketMs = 0;
  bool _hasAnySignal = false;

  static constexpr size_t BUF_SIZE = 512;
  uint8_t _buf[BUF_SIZE];

  static constexpr uint32_t SIGNAL_TIMEOUT_MS = 300;
};
