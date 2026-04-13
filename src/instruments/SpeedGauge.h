#pragma once
#include <Arduino.h>
#include "drivers/motors/GaugeMotorTmc2208.h"

class SpeedGauge {
public:
  struct Config {
    float maxSpeedKmh = 220.0f;
    int32_t minSteps = 0;
    int32_t maxSteps = 180;
    float normalStepsPerSec = 60.0f;
    float fastStepsPerSec = 800.0f;

    // ===== Calibração própria do velocímetro =====
    int32_t calibrationBackoffSteps = 8;
    float calibrationRiseStepsPerSec = 300.0f;
    float calibrationFallStepsPerSec = 300.0f;
  };

  SpeedGauge(GaugeMotorTmc2208& motor, const Config& cfg);

  void begin();
  void calibrate();
  void setSpeedKmh(float speedKmh);
  void tick(uint32_t nowMicros);

private:
  float clampSpeed(float speedKmh) const;
  int32_t speedToSteps(float speedKmh) const;

  GaugeMotorTmc2208& _motor;
  Config _cfg;

  float _currentSpeedKmh = 0.0f;
  int32_t _targetSteps = 0;
};