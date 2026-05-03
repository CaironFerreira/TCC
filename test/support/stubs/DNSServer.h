#pragma once

#include <stdint.h>

#include "WiFi.h"

namespace dnsserver_stub {

struct State {
  uint16_t lastPort = 0;
  String lastDomain = "";
  IPAddress lastIp;
  uint32_t startCalls = 0;
  uint32_t processCalls = 0;
  uint32_t stopCalls = 0;
};

inline State& state() {
  static State value;
  return value;
}

inline void reset() {
  state() = State{};
}

}  // namespace dnsserver_stub

class DNSServer {
public:
  void start(uint16_t port, const char* domainName, const IPAddress& ip) {
    dnsserver_stub::state().lastPort = port;
    dnsserver_stub::state().lastDomain = domainName;
    dnsserver_stub::state().lastIp = ip;
    ++dnsserver_stub::state().startCalls;
  }

  void processNextRequest() {
    ++dnsserver_stub::state().processCalls;
  }

  void stop() {
    ++dnsserver_stub::state().stopCalls;
  }
};
