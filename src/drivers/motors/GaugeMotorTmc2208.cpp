#include "GaugeMotorTmc2208.h"

GaugeMotorTmc2208::GaugeMotorTmc2208(const Config& cfg)
: _cfg(cfg) {}

void GaugeMotorTmc2208::begin() {
  if (_cfg.pinStep < 0 || _cfg.pinDirection < 0) {
    return;
  }

  pinMode(_cfg.pinStep, OUTPUT);
  pinMode(_cfg.pinDirection, OUTPUT);

  if (_cfg.pinEnable >= 0) {
    pinMode(_cfg.pinEnable, OUTPUT);
  }

  digitalWrite(_cfg.pinStep, LOW);
  setDirection(true);
  enable(true);

  _lastUpdateMicros = micros();
  _stepAccumulator = 0.0f;
}

void GaugeMotorTmc2208::enable(bool on) {
  _enabled = on;

  if (_cfg.pinEnable >= 0) {
    // TMC2208 normalmente habilita em LOW
    digitalWrite(_cfg.pinEnable, on ? LOW : HIGH);
  }
}

void GaugeMotorTmc2208::setDirection(bool forward) {
  bool level = forward;

  if (_cfg.invertDirection) {
    level = !level;
  }

  digitalWrite(_cfg.pinDirection, level ? HIGH : LOW);
}

void GaugeMotorTmc2208::pulseStep() {
  digitalWrite(_cfg.pinStep, HIGH);
  delayMicroseconds(_cfg.stepPulseUs);
  digitalWrite(_cfg.pinStep, LOW);
}

void GaugeMotorTmc2208::moveTo(int32_t targetSteps, float riseStepsPerSec, float fallStepsPerSec) {
  _targetPositionSteps = targetSteps;

  _riseStepsPerSec = (riseStepsPerSec > 0.0f) ? riseStepsPerSec : 1.0f;
  _fallStepsPerSec = (fallStepsPerSec > 0.0f) ? fallStepsPerSec : 1.0f;

  if (_targetPositionSteps >= _currentPositionSteps) {
    _activeStepsPerSec = _riseStepsPerSec;
  } else {
    _activeStepsPerSec = _fallStepsPerSec;
  }
}

void GaugeMotorTmc2208::setCurrentPosition(int32_t steps) {
  _currentPositionSteps = steps;
  _targetPositionSteps = steps;
  _stepAccumulator = 0.0f;
  _lastUpdateMicros = micros();
}

bool GaugeMotorTmc2208::isMoving() const {
  return _currentPositionSteps != _targetPositionSteps;
}

void GaugeMotorTmc2208::tick(uint32_t nowMicros) {
  if (!_enabled) {
    return;
  }

  if (_currentPositionSteps == _targetPositionSteps) {
    _lastUpdateMicros = nowMicros;
    _stepAccumulator = 0.0f;
    return;
  }

  const uint32_t deltaMicros = nowMicros - _lastUpdateMicros;
  _lastUpdateMicros = nowMicros;

  const float deltaSteps = (_activeStepsPerSec * (float)deltaMicros) / 1000000.0f;
  _stepAccumulator += deltaSteps;

  while (_stepAccumulator >= 1.0f && _currentPositionSteps != _targetPositionSteps) {
    if (_targetPositionSteps > _currentPositionSteps) {
      setDirection(true);
      pulseStep();
      _currentPositionSteps++;
    } else {
      setDirection(false);
      pulseStep();
      _currentPositionSteps--;
    }

    _stepAccumulator -= 1.0f;
  }
}