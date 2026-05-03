#pragma once

#include <stdint.h>

class IInstrumentCluster {
public:
  virtual ~IInstrumentCluster() = default;

  virtual void begin() = 0;
  virtual void setSpeedKmh(float speedKmh) = 0;
  virtual void setRpm(float rpm) = 0;
  virtual void setFuelLevel(float fuelLevel) = 0;
  virtual void setTireTemperature(float temperatureC) = 0;
  virtual void tick(uint32_t nowMicros) = 0;
};
