#include "RpmGauge.h"
#include <math.h>

RpmGauge::RpmGauge(GaugeMotorTmc2208& motor, const Config& cfg)
: _motor(motor), _cfg(cfg) {}

void RpmGauge::begin() {
  _currentRpm = 0.0f;
  _targetSteps = rpmToSteps(0.0f);

  const float speed = _cfg.normalStepsPerSec;
  _motor.moveTo(_targetSteps, speed, speed);
}

void RpmGauge::setRpm(float rpm) {
  const float clampedRpm = clampRpm(rpm);

  const int32_t rawTargetSteps = rpmToSteps(clampedRpm);
  const int32_t distanceToTarget = rawTargetSteps - _motor.currentPosition();

  const bool fastDrop = distanceToTarget < -35;
  const bool fastRise = distanceToTarget > 35;

  if (fastDrop || fastRise) {
    _currentRpm = clampedRpm;
  } else {
    const float alpha = 0.25f;
    _currentRpm = _currentRpm + alpha * (clampedRpm - _currentRpm);
  }

  _targetSteps = rpmToSteps(_currentRpm);

  const float speedToUse =
      (fastDrop || fastRise) ? _cfg.fastStepsPerSec : _cfg.normalStepsPerSec;

  _motor.moveTo(_targetSteps, speedToUse, speedToUse);
}

void RpmGauge::tick(uint32_t nowMicros) {
  _motor.tick(nowMicros);
}

float RpmGauge::clampRpm(float rpm) const {
  if (!isfinite(rpm)) {
    return 0.0f;
  }

  if (rpm < 0.0f) {
    return 0.0f;
  }

  if (rpm > _cfg.maxRpm) {
    return _cfg.maxRpm;
  }

  return rpm;
}

int32_t RpmGauge::rpmToSteps(float rpm) const {
  const float clampedRpm = clampRpm(rpm);

  const int32_t rangeSteps = _cfg.maxSteps - _cfg.minSteps;
  if (_cfg.maxRpm <= 0.0f) {
    return _cfg.minSteps;
  }

  const float ratio = clampedRpm / _cfg.maxRpm;
  const float stepsFloat = (float)_cfg.minSteps + (ratio * (float)rangeSteps);

  return (int32_t)lroundf(stepsFloat);
}