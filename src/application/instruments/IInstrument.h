#pragma once

#include <stdint.h>

enum class InstrumentSignalType : uint8_t {
  SpeedKmh,
  Rpm,
  FuelLevel,
  TireTemperatureC
};

struct InstrumentSignal {
  InstrumentSignalType type = InstrumentSignalType::SpeedKmh;
  float value = 0.0f;
};

class IInstrument {
public:
  virtual ~IInstrument() = default;

  virtual void begin() = 0;
  virtual void tick(uint32_t nowMicros) = 0;
  virtual bool supports(InstrumentSignalType type) const = 0;
  virtual void apply(const InstrumentSignal& signal) = 0;
};
