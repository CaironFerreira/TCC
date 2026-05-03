#pragma once
#include <WiFi.h>
#include <WiFiUdp.h>
#include <stddef.h>
#include <stdint.h>
#include "ports/IPacketReceiver.h"

class UdpReceiver : public IPacketReceiver {
public:
  bool begin(uint16_t port) override;
  PacketReceiveResult receive(uint8_t* buffer, size_t bufferSize) override;

private:
  WiFiUDP _udp;
  uint16_t _port = 0;
};
