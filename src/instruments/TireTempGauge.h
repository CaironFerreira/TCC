#pragma once
#include <Arduino.h>
#include "ports/IGaugeMotor.h"

class TireTempGauge {
public:
  struct Config {
    float minTemp = 20.0f;
    float maxTemp = 120.0f;

    int32_t minSteps = 0;
    int32_t maxSteps = 180;

    float normalStepsPerSec = 180.0f;
    float fastStepsPerSec = 220.0f;

    float calibrationRiseStepsPerSec = 300.0f;
    float calibrationFallStepsPerSec = 300.0f;
  };

  TireTempGauge(IGaugeMotor& motor, const Config& cfg);

  void begin();
  void calibrate();
  void setTemperature(float temp);
  void tick(uint32_t nowMicros);

  float currentTemperature() const { return _currentTemp; }
  int32_t targetSteps() const { return _targetSteps; }

private:
  float clampTemp(float temp) const;
  int32_t tempToSteps(float temp) const;

  IGaugeMotor& _motor;
  Config _cfg;

  float _currentTemp = 20.0f;
  int32_t _targetSteps = 0;
};
