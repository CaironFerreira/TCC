#pragma once
#include <WiFi.h>
#include <WiFiUdp.h>
#include <stddef.h>
#include <stdint.h>

class UdpReceiver {
public:
  bool begin(uint16_t port);
  int receive(uint8_t* buffer, size_t bufferSize); // retorna bytes recebidos ou 0 se nada

private:
  WiFiUDP _udp;
  uint16_t _port = 0;
};
