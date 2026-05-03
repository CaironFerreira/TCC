#include "application/instruments/TireTempGauge.h"
#include <math.h>

TireTempGauge::TireTempGauge(IGaugeMotor& motor, IClock& clock, const Config& cfg)
: _motor(motor), _clock(clock), _cfg(cfg) {}

void TireTempGauge::begin() {
  _motor.begin();
  calibrate();

  _currentTemp = _cfg.minTemp;
  _targetSteps = tempToSteps(_currentTemp);
  _hasTemperature = false;

  const float speed = _cfg.normalStepsPerSec;
  _motor.moveTo(_targetSteps, speed, speed);
}

void TireTempGauge::calibrate() {
  // zera referência atual
  _motor.setCurrentPosition(0);

  // recua além do curso para garantir batente
  _motor.moveTo(
    -(int32_t)(_cfg.maxSteps * 1.3f),
    _cfg.calibrationRiseStepsPerSec,
    _cfg.calibrationFallStepsPerSec
  );

  while (_motor.isMoving()) {
    _motor.tick(_clock.nowMicros());
  }

  // afasta do batente para definir uma posição inicial estável
  _motor.setCurrentPosition(0);
  if (_cfg.calibrationBackoffSteps > 0) {
    _motor.moveTo(
      _cfg.calibrationBackoffSteps,
      _cfg.calibrationRiseStepsPerSec,
      _cfg.calibrationFallStepsPerSec
    );

    while (_motor.isMoving()) {
      _motor.tick(_clock.nowMicros());
    }
  }

  // define zero lógico depois do recuo
  _motor.setCurrentPosition(0);

  _currentTemp = _cfg.minTemp;
  _targetSteps = 0;
  _hasTemperature = false;
}

void TireTempGauge::setTemperature(float temp) {
  const float clamped = clampTemp(temp);

  if (!_hasTemperature) {
    _currentTemp = clamped;
    _hasTemperature = true;
  } else {
    const float alpha = 0.15f;
    _currentTemp = _currentTemp + alpha * (clamped - _currentTemp);
  }

  _targetSteps = tempToSteps(_currentTemp);
  _motor.moveTo(_targetSteps, _cfg.normalStepsPerSec, _cfg.fastStepsPerSec);
}

void TireTempGauge::tick(uint32_t nowMicros) {
  _motor.tick(nowMicros);
}

bool TireTempGauge::supports(InstrumentSignalType type) const {
  return type == InstrumentSignalType::TireTemperatureC;
}

void TireTempGauge::apply(const InstrumentSignal& signal) {
  if (signal.type == InstrumentSignalType::TireTemperatureC) {
    setTemperature(signal.value);
  }
}

float TireTempGauge::clampTemp(float temp) const {
  if (!isfinite(temp)) {
    return _cfg.minTemp;
  }

  if (temp < _cfg.minTemp) {
    return _cfg.minTemp;
  }

  if (temp > _cfg.maxTemp) {
    return _cfg.maxTemp;
  }

  return temp;
}

int32_t TireTempGauge::tempToSteps(float temp) const {
  const float clamped = clampTemp(temp);
  const float rangeTemp = _cfg.maxTemp - _cfg.minTemp;
  const float rangeSteps = fabsf((float)(_cfg.maxSteps - _cfg.minSteps));

  if (rangeTemp <= 0.0f) {
    return _cfg.minSteps;
  }

  const float ratio = (clamped - _cfg.minTemp) / rangeTemp;
  const float direction = _cfg.invertIndicationDirection ? -1.0f : 1.0f;
  const float stepsFloat = (float)_cfg.minSteps + direction * ratio * rangeSteps;

  return (int32_t)lroundf(stepsFloat);
}
