#pragma once

#include <stdint.h>

enum class TelemetryInputStatus : uint8_t {
  Idle,
  NoPacket,
  PacketReceived,
  PacketDiscarded,
  ReceiveError
};
