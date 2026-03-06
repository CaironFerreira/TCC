#include "GaugeMotorTmc2208.h"

GaugeMotorTmc2208::GaugeMotorTmc2208(const Config& cfg)
: _cfg(cfg) {}

void GaugeMotorTmc2208::begin() {
  // Segurança básica
  if (_cfg.pinStep < 0 || _cfg.pinDirection < 0) return;

  pinMode(_cfg.pinStep, OUTPUT);
  pinMode(_cfg.pinDirection, OUTPUT);

  if (_cfg.pinEnable >= 0) {
    pinMode(_cfg.pinEnable, OUTPUT);
  }

  // Estados iniciais
  digitalWrite(_cfg.pinStep, LOW);
  setDirection(true);

  enable(true);
  _lastStepMicros = micros();
}

void GaugeMotorTmc2208::enable(bool on) {
  _enabled = on;

  if (_cfg.pinEnable < 0) return;

  // Padrão mais comum em drivers: EN ativo em LOW (LOW = habilita)
  digitalWrite(_cfg.pinEnable, on ? LOW : HIGH);
}

void GaugeMotorTmc2208::setCurrentSteps(int32_t steps) {
  _posSteps = steps;
}

void GaugeMotorTmc2208::moveTo(int32_t targetSteps) {
  _targetSteps = targetSteps;
}

void GaugeMotorTmc2208::setDirection(bool forward) {
  _dirForward = forward;

  bool level = forward;
  if (_cfg.invertDirection) level = !level;

  digitalWrite(_cfg.pinDirection, level ? HIGH : LOW);
}

void GaugeMotorTmc2208::pulseStep() {
  // Pulso STEP: HIGH por stepPulseUs, depois LOW
  digitalWrite(_cfg.pinStep, HIGH);
  delayMicroseconds(_cfg.stepPulseUs);
  digitalWrite(_cfg.pinStep, LOW);
}

void GaugeMotorTmc2208::tick(uint32_t nowMicros) {
  if (!_enabled) return;

  const int32_t delta = _targetSteps - _posSteps;
  if (delta == 0) return;

  // Rate limit: só dá passo se passou o intervalo mínimo
  const uint32_t elapsed = nowMicros - _lastStepMicros;
  if (elapsed < _cfg.stepIntervalUs) return;

  const bool forward = (delta > 0);

  // Só muda DIR quando necessário
  if (forward != _dirForward) {
    setDirection(forward);
    // Pequena margem antes do STEP após troca de DIR (setup time).
    // 2us costuma ser suficiente; mantemos simples e barato.
    delayMicroseconds(2);
  }

  pulseStep();
  _posSteps += forward ? 1 : -1;

  _lastStepMicros = nowMicros;
}