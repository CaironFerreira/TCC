#include "application/instruments/SpeedGauge.h"
#include <math.h>

SpeedGauge::SpeedGauge(IGaugeMotor& motor, IClock& clock, const Config& cfg)
: _motor(motor), _clock(clock), _cfg(cfg) {}

void SpeedGauge::begin() {
  _motor.begin();
  calibrate();

  _currentSpeedKmh = 0.0f;
  _targetSteps = speedToSteps(0.0f);

  const float speed = _cfg.normalStepsPerSec;
  _motor.moveTo(_targetSteps, speed, speed);
}

void SpeedGauge::calibrate() {

  // zera referência atual
  _motor.setCurrentPosition(0);

  _motor.moveTo(
    -(_cfg.maxSteps*1.3f),
    _cfg.calibrationRiseStepsPerSec,
    _cfg.calibrationFallStepsPerSec
  );
  while (_motor.isMoving()) {
    _motor.tick(_clock.nowMicros());
  }
  _motor.setCurrentPosition(0);
  _motor.moveTo(
      10,
      _cfg.calibrationRiseStepsPerSec,
      _cfg.calibrationFallStepsPerSec
  );
  while (_motor.isMoving()) {
    _motor.tick(_clock.nowMicros());
  }
  // define essa posição como zero lógico
  _motor.setCurrentPosition(0);

  _currentSpeedKmh = 0.0f;
  _targetSteps = 0;
}

void SpeedGauge::setSpeedKmh(float speedKmh) {
  const float clamped = clampSpeed(speedKmh);

  // alvo bruto da nova leitura
  const int32_t rawTargetSteps = speedToSteps(clamped);

  // distância real entre ponteiro e novo alvo
  const int32_t distanceToTarget = rawTargetSteps - _motor.currentPosition();

  const bool fastDrop = distanceToTarget < -20;
  const bool fastRise = distanceToTarget > 20;

  // em queda brusca, vai direto para o valor novo
  if (fastDrop) {
    _currentSpeedKmh = clamped;
  } else {
    const float alpha = 0.25f;
    _currentSpeedKmh = _currentSpeedKmh + alpha * (clamped - _currentSpeedKmh);
  }

  _targetSteps = speedToSteps(_currentSpeedKmh);

  const float speedToUse =
      (fastDrop || fastRise) ? _cfg.fastStepsPerSec : _cfg.normalStepsPerSec;

  _motor.moveTo(_targetSteps, speedToUse, speedToUse);
}

void SpeedGauge::tick(uint32_t nowMicros) {
  _motor.tick(nowMicros);
}

bool SpeedGauge::supports(InstrumentSignalType type) const {
  return type == InstrumentSignalType::SpeedKmh;
}

void SpeedGauge::apply(const InstrumentSignal& signal) {
  if (signal.type == InstrumentSignalType::SpeedKmh) {
    setSpeedKmh(signal.value);
  }
}

float SpeedGauge::clampSpeed(float speedKmh) const {
  if (!isfinite(speedKmh) || speedKmh < 0.0f) {
    return 0.0f;
  }

  if (speedKmh > _cfg.maxSpeedKmh) {
    return _cfg.maxSpeedKmh;
  }

  return speedKmh;
}

int32_t SpeedGauge::speedToSteps(float speedKmh) const {
  const float clamped = clampSpeed(speedKmh);

  const int32_t rangeSteps = _cfg.maxSteps - _cfg.minSteps;
  if (_cfg.maxSpeedKmh <= 0.0f) {
    return _cfg.minSteps;
  }

  const float ratio = clamped / _cfg.maxSpeedKmh;
  const float stepsFloat = (float)_cfg.minSteps + ratio * (float)rangeSteps;

  return (int32_t)lroundf(stepsFloat);
}
