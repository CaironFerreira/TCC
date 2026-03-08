#include "SpeedGauge.h"

SpeedGauge::SpeedGauge(GaugeMotorTmc2208& motor, const Config& cfg)
: _motor(motor), _cfg(cfg) {}

void SpeedGauge::begin() {
  _currentSpeedKmh = 0.0f;
  _targetSteps = speedToSteps(0.0f);
  _motor.moveTo(_targetSteps);
}

void SpeedGauge::setSpeedKmh(float speedKmh) {
  const float clampedSpeed = clampSpeed(speedKmh);

  // Filtro exponencial simples para suavizar a leitura
  const float alpha = 0.06f;
  _currentSpeedKmh = _currentSpeedKmh + alpha * (clampedSpeed - _currentSpeedKmh);

  _targetSteps = speedToSteps(_currentSpeedKmh);
  _motor.moveTo(_targetSteps);
}

void SpeedGauge::tick(uint32_t nowMicros) {
  _motor.tick(nowMicros);
}

float SpeedGauge::clampSpeed(float speedKmh) const {
  if (!isfinite(speedKmh)) {
    return 0.0f;
  }

  if (speedKmh < 0.0f) {
    return 0.0f;
  }

  if (speedKmh > _cfg.maxSpeedKmh) {
    return _cfg.maxSpeedKmh;
  }

  return speedKmh;
}

int32_t SpeedGauge::speedToSteps(float speedKmh) const {
  const float clampedSpeed = clampSpeed(speedKmh);

  const int32_t rangeSteps = _cfg.maxSteps - _cfg.minSteps;
  if (_cfg.maxSpeedKmh <= 0.0f) {
    return _cfg.minSteps;
  }

  const float ratio = clampedSpeed / _cfg.maxSpeedKmh;
  const float stepsFloat = (float)_cfg.minSteps + (ratio * (float)rangeSteps);

  return (int32_t)lroundf(stepsFloat);
}