#pragma once
#include "ITelemetryDecoder.h"

class Forza7Decoder : public ITelemetryDecoder {
public:
  bool decode(const uint8_t* data, size_t len, TelemetryFrame& out) override;
  const char* name() const override { return "Forza Motorsport 7 (Data Out)"; }

};
