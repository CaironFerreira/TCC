#pragma once

#include <stddef.h>
#include <stdint.h>

enum class PacketReceiveStatus : uint8_t {
  NoPacket,
  PacketReceived,
  PacketTooLarge,
  ReceiveError
};

struct PacketReceiveResult {
  PacketReceiveStatus status = PacketReceiveStatus::NoPacket;
  size_t bytesRead = 0;
};

class IPacketReceiver {
public:
  virtual ~IPacketReceiver() = default;

  virtual bool begin(uint16_t port) = 0;
  virtual PacketReceiveResult receive(uint8_t* buffer, size_t bufferSize) = 0;
};
