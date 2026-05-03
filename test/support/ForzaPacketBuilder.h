#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace test_support {

static constexpr size_t FORZA_DASH_PACKET_SIZE = 311;

class ForzaPacketBuilder {
public:
  ForzaPacketBuilder() { memset(_data, 0, sizeof(_data)); }

  void writeF32(size_t offset, float value) {
    memcpy(_data + offset, &value, sizeof(value));
  }

  void writeU16(size_t offset, uint16_t value) {
    _data[offset] = (uint8_t)(value & 0xFFU);
    _data[offset + 1] = (uint8_t)((value >> 8) & 0xFFU);
  }

  void writeU8(size_t offset, uint8_t value) {
    _data[offset] = value;
  }

  const uint8_t* data() const { return _data; }
  size_t size() const { return sizeof(_data); }

private:
  uint8_t _data[FORZA_DASH_PACKET_SIZE];
};

}  // namespace test_support
