#include "adapters/network/udp/UdpReceiver.h"

bool UdpReceiver::begin(uint16_t port) {
  _port = port;
  return _udp.begin(_port) == 1;
}

PacketReceiveResult UdpReceiver::receive(uint8_t* buffer, size_t bufferSize) {
  int packetSize = _udp.parsePacket();
  if (packetSize <= 0) {
    PacketReceiveResult result;
    result.status = PacketReceiveStatus::NoPacket;
    return result;
  }

  if ((size_t)packetSize > bufferSize) {
    // descarta se for maior que o buffer
    while (_udp.available()) _udp.read();
    PacketReceiveResult result;
    result.status = PacketReceiveStatus::PacketTooLarge;
    return result;
  }

  const int bytesRead = _udp.read(buffer, packetSize);
  if (bytesRead <= 0) {
    PacketReceiveResult result;
    result.status = PacketReceiveStatus::ReceiveError;
    return result;
  }

  PacketReceiveResult result;
  result.status = PacketReceiveStatus::PacketReceived;
  result.bytesRead = (size_t)bytesRead;
  return result;
}
