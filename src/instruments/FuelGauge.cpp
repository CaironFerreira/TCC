#include "instruments/FuelGauge.h"
#include <math.h>

FuelGauge::FuelGauge(IGaugeMotor& motor, const Config& cfg)
: _motor(motor), _cfg(cfg) {}

void FuelGauge::begin() {
  _motor.begin();
  calibrate();

  _currentFuelLevel = 0.0f;
  _targetSteps = fuelToSteps(0.0f);

  const float speed = _cfg.normalStepsPerSec;
  _motor.moveTo(_targetSteps, speed, speed);
}

void FuelGauge::calibrate() {
  // zera referência atual
  _motor.setCurrentPosition(0);

  const int32_t calibrationSteps = (int32_t)(_cfg.maxSteps * 1.3f);
  const int32_t calibrationTarget =
      _cfg.invertCalibrationDirection ? calibrationSteps : -calibrationSteps;

  // recua além do curso para garantir batente
  _motor.moveTo(
    calibrationTarget,
    _cfg.calibrationRiseStepsPerSec,
    _cfg.calibrationFallStepsPerSec
  );

  while (_motor.isMoving()) {
    _motor.tick(micros());
  }

  // define essa posição como zero lógico
  _motor.setCurrentPosition(0);

  _currentFuelLevel = 0.0f;
  _targetSteps = 0;
}

void FuelGauge::setFuelLevel(float level) {
  const float clamped = clampFuelLevel(level);

  _currentFuelLevel = clamped;
  _targetSteps = fuelToSteps(_currentFuelLevel);
  _motor.moveTo(_targetSteps, _cfg.normalStepsPerSec, _cfg.fastStepsPerSec);
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
  const float rangeSteps = fabsf((float)(_cfg.maxSteps - _cfg.minSteps));

  if (rangeValue <= 0.0f) {
    return _cfg.minSteps;
  }

  const float ratio = (clamped - _cfg.minFuelLevel) / rangeValue;
  const float direction = _cfg.invertIndicationDirection ? -1.0f : 1.0f;
  const float stepsFloat = (float)_cfg.minSteps + direction * ratio * rangeSteps;

  return (int32_t)lroundf(stepsFloat);
}
