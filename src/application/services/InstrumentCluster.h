#pragma once

#include <stddef.h>
#include "application/instruments/IInstrument.h"
#include "ports/IInstrumentCluster.h"

class InstrumentCluster : public IInstrumentCluster {
public:
  InstrumentCluster(IInstrument* const* instruments, size_t instrumentCount);

  void begin() override;
  void setSpeedKmh(float speedKmh) override;
  void setRpm(float rpm) override;
  void setFuelLevel(float fuelLevel) override;
  void setTireTemperature(float temperatureC) override;
  void tick(uint32_t nowMicros) override;

private:
  void dispatchSignal(InstrumentSignalType type, float value);

private:
  IInstrument* const* _instruments = nullptr;
  size_t _instrumentCount = 0;
};
