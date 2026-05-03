#pragma once

#include <deque>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <vector>

namespace wifiudp_stub {

struct PacketEvent {
  int reportedSize = 0;
  std::vector<uint8_t> data;
  int readResult = -2;
};

struct State {
  int beginResult = 1;
  uint16_t beginPort = 0;
  uint32_t beginCalls = 0;
  uint32_t parsePacketCalls = 0;
  uint32_t readBufferCalls = 0;
  uint32_t readByteCalls = 0;
  uint32_t bytesConsumed = 0;
  std::deque<PacketEvent> queued;
  bool hasActivePacket = false;
  PacketEvent activePacket;
  size_t readOffset = 0;
};

inline State& state() {
  static State value;
  return value;
}

inline void reset() {
  state() = State{};
}

inline void setBeginResult(int result) {
  state().beginResult = result;
}

inline void enqueuePacket(const uint8_t* data, size_t size) {
  PacketEvent event;
  event.reportedSize = (int)size;
  event.data.assign(data, data + size);
  state().queued.push_back(event);
}

inline void enqueuePacketWithReadError(const uint8_t* data, size_t size) {
  PacketEvent event;
  event.reportedSize = (int)size;
  event.data.assign(data, data + size);
  event.readResult = -1;
  state().queued.push_back(event);
}

inline void enqueueOversizedPacket(size_t reportedSize) {
  PacketEvent event;
  event.reportedSize = (int)reportedSize;
  event.data.resize(reportedSize, 0xAB);
  state().queued.push_back(event);
}

inline uint16_t beginPort() { return state().beginPort; }
inline uint32_t beginCalls() { return state().beginCalls; }
inline uint32_t parsePacketCalls() { return state().parsePacketCalls; }
inline uint32_t readBufferCalls() { return state().readBufferCalls; }
inline uint32_t readByteCalls() { return state().readByteCalls; }
inline uint32_t bytesConsumed() { return state().bytesConsumed; }

}  // namespace wifiudp_stub

class WiFiUDP {
public:
  int begin(uint16_t port) {
    wifiudp_stub::state().beginPort = port;
    ++wifiudp_stub::state().beginCalls;
    return wifiudp_stub::state().beginResult;
  }

  int parsePacket() {
    ++wifiudp_stub::state().parsePacketCalls;

    if (wifiudp_stub::state().queued.empty()) {
      wifiudp_stub::state().hasActivePacket = false;
      wifiudp_stub::state().readOffset = 0;
      return 0;
    }

    wifiudp_stub::state().activePacket = wifiudp_stub::state().queued.front();
    wifiudp_stub::state().queued.pop_front();
    wifiudp_stub::state().hasActivePacket = true;
    wifiudp_stub::state().readOffset = 0;
    return wifiudp_stub::state().activePacket.reportedSize;
  }

  int available() {
    if (!wifiudp_stub::state().hasActivePacket) {
      return 0;
    }

    const size_t remaining =
        wifiudp_stub::state().activePacket.data.size() - wifiudp_stub::state().readOffset;
    return (int)remaining;
  }

  int read(uint8_t* buffer, size_t len) {
    ++wifiudp_stub::state().readBufferCalls;

    if (!wifiudp_stub::state().hasActivePacket) {
      return 0;
    }

    if (wifiudp_stub::state().activePacket.readResult == -1) {
      wifiudp_stub::state().hasActivePacket = false;
      wifiudp_stub::state().readOffset = 0;
      return -1;
    }

    const size_t remaining =
        wifiudp_stub::state().activePacket.data.size() - wifiudp_stub::state().readOffset;
    const size_t toCopy = (len < remaining) ? len : remaining;

    if (buffer != nullptr && toCopy > 0) {
      memcpy(buffer,
             wifiudp_stub::state().activePacket.data.data() + wifiudp_stub::state().readOffset,
             toCopy);
    }

    wifiudp_stub::state().readOffset += toCopy;
    wifiudp_stub::state().bytesConsumed += (uint32_t)toCopy;
    wifiudp_stub::state().hasActivePacket = false;
    wifiudp_stub::state().readOffset = 0;
    return (int)toCopy;
  }

  int read() {
    ++wifiudp_stub::state().readByteCalls;

    if (!wifiudp_stub::state().hasActivePacket) {
      return -1;
    }

    if (wifiudp_stub::state().readOffset >= wifiudp_stub::state().activePacket.data.size()) {
      wifiudp_stub::state().hasActivePacket = false;
      wifiudp_stub::state().readOffset = 0;
      return -1;
    }

    const int value =
        wifiudp_stub::state().activePacket.data[wifiudp_stub::state().readOffset++];
    ++wifiudp_stub::state().bytesConsumed;

    if (wifiudp_stub::state().readOffset >= wifiudp_stub::state().activePacket.data.size()) {
      wifiudp_stub::state().hasActivePacket = false;
      wifiudp_stub::state().readOffset = 0;
    }

    return value;
  }
};
