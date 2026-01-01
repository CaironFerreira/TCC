#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../domain/TelemetryFrame.h"
#include "../drivers/net/UdpReceiver.h"
#include "../decoders/ITelemetryDecoder.h"

class TelemetryService {
public:
  TelemetryService(UdpReceiver& receiver, ITelemetryDecoder& decoder);

  bool begin(uint16_t port);
  void tick(); // chama no loop()

  const TelemetryFrame& lastFrame() const { return _last; }

private:
  UdpReceiver& _receiver;
  ITelemetryDecoder& _decoder;

  TelemetryFrame _last;
  uint32_t _lastPacketMs = 0;
  uint32_t _lastAnyPacketMs = 0;
  bool _hasAnySignal = false;

  static constexpr size_t BUF_SIZE = 512;
  uint8_t _buf[BUF_SIZE];

  static constexpr uint32_t SIGNAL_TIMEOUT_MS = 300; // ajustável
};
