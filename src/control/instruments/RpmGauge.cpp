#include "RpmGauge.h"

RpmGauge::RpmGauge(GaugeMotorTmc2208& motor, const Config& cfg)
: _motor(motor), _cfg(cfg) {}

void RpmGauge::begin() {
  _currentRpm = 0.0f;
  _targetSteps = rpmToSteps(0.0f);
  _motor.moveTo(_targetSteps);
}

void RpmGauge::setRpm(float rpm) {
  const float clampedRpm = clampRpm(rpm);

  // Filtro exponencial simples para suavizar a leitura
  const float alpha = 0.06f;
  _currentRpm = _currentRpm + alpha * (clampedRpm - _currentRpm);

  _targetSteps = rpmToSteps(_currentRpm);
  _motor.moveTo(_targetSteps);
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