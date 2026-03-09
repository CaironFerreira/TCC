#pragma once
#include <Arduino.h>

#include "drivers/motors/GaugeMotorTmc2208.h"

class RpmGauge {
public:
  struct Config {
    float maxRpm = 8000.0f;   // limite da escala do conta-giros
    int32_t minSteps = 0;
    int32_t maxSteps = 180;
  };

  RpmGauge(GaugeMotorTmc2208& motor, const Config& cfg);

  void begin();

  void setRpm(float rpm);

  void tick(uint32_t nowMicros);

  float currentRpm() const { return _currentRpm; }
  int32_t targetSteps() const { return _targetSteps; }

private:
  int32_t rpmToSteps(float rpm) const;
  float clampRpm(float rpm) const;

  GaugeMotorTmc2208& _motor;
  Config _cfg;

  float _currentRpm = 0.0f;
  int32_t _targetSteps = 0;
};