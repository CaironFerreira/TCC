#pragma once
#include <Arduino.h>
#include "drivers/motors/GaugeMotorTmc2208.h"

class TireTempGauge {
public:
  struct Config {
    float minTemp = 20.0f;
    float maxTemp = 120.0f;
    int32_t minSteps = 0;
    int32_t maxSteps = 180;
    float riseStepsPerSec = 180.0f;
    float fallStepsPerSec = 220.0f;
  };

  TireTempGauge(GaugeMotorTmc2208& motor, const Config& cfg);

  void begin();
  void setTemperature(float temp);
  void tick(uint32_t nowMicros);

private:
  float clampTemp(float temp) const;
  int32_t tempToSteps(float temp) const;

  GaugeMotorTmc2208& _motor;
  Config _cfg;

  float _currentTemp = 20.0f;
  int32_t _targetSteps = 0;
};