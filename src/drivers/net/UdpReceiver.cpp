#include "UdpReceiver.h"

bool UdpReceiver::begin(uint16_t port) {
  _port = port;
  return _udp.begin(_port) == 1;
}

int UdpReceiver::receive(uint8_t* buffer, size_t bufferSize) {
  int packetSize = _udp.parsePacket();
  if (packetSize <= 0) return 0;
   
  if ((size_t)packetSize > bufferSize) {
    // descarta se for maior que o buffer
    while (_udp.available()) _udp.read();
    return -1;
  }

  return _udp.read(buffer, packetSize);
}
