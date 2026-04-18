#pragma once

#include <stddef.h>
#include <stdint.h>

class IPacketReceiver {
public:
  virtual ~IPacketReceiver() = default;

  virtual bool begin(uint16_t port) = 0;
  virtual int receive(uint8_t* buffer, size_t bufferSize) = 0;
};
