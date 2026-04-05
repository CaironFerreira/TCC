#include "FuelGauge.h"
#include <math.h>

FuelGauge::FuelGauge(GaugeMotorTmc2208& motor, const Config& cfg)
: _motor(motor), _cfg(cfg) {}

void FuelGauge::begin() {
  _currentFuelLevel = 0.0f;
  _targetSteps = fuelToSteps(0.0f);
  _motor.moveTo(_targetSteps, _cfg.riseStepsPerSec, _cfg.fallStepsPerSec);
}

void FuelGauge::setFuelLevel(float level) {
  const float clamped = clampFuelLevel(level);

  const float alpha = 0.10f;
  _currentFuelLevel = _currentFuelLevel + alpha * (clamped - _currentFuelLevel);

  _targetSteps = fuelToSteps(_currentFuelLevel);
  _motor.moveTo(_targetSteps, _cfg.riseStepsPerSec, _cfg.fallStepsPerSec);
}

void FuelGauge::tick(uint32_t nowMicros) {
  _motor.tick(nowMicros);
}

float FuelGauge::clampFuelLevel(float level) const {
  if (!isfinite(level)) {
    return _cfg.minFuelLevel;
  }

  if (level < _cfg.minFuelLevel) {
    return _cfg.minFuelLevel;
  }

  if (level > _cfg.maxFuelLevel) {
    return _cfg.maxFuelLevel;
  }

  return level;
}

int32_t FuelGauge::fuelToSteps(float level) const {
  const float clamped = clampFuelLevel(level);
  const float rangeValue = _cfg.maxFuelLevel - _cfg.minFuelLevel;
  const int32_t rangeSteps = _cfg.maxSteps - _cfg.minSteps;

  if (rangeValue <= 0.0f) {
    return _cfg.minSteps;
  }

  const float ratio = (clamped - _cfg.minFuelLevel) / rangeValue;
  const float stepsFloat = (float)_cfg.minSteps + ratio * (float)rangeSteps;

  return (int32_t)lroundf(stepsFloat);
}