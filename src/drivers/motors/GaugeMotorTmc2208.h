#pragma once
#include <Arduino.h>
#include "ports/IGaugeMotor.h"

class GaugeMotorTmc2208 : public IGaugeMotor {
public:
  struct Config {
    int pinStep = -1;
    int pinDirection = -1;
    int pinEnable = -1;
    bool invertDirection = false;
    uint32_t stepPulseUs = 2;
  };

  explicit GaugeMotorTmc2208(const Config& cfg);

  void begin() override;
  void enable(bool on) override;
  void tick(uint32_t nowMicros) override;

  // Nova arquitetura: cada comando já informa as velocidades de subida/descida
  void moveTo(int32_t targetSteps, float riseStepsPerSec, float fallStepsPerSec) override;

  // Usado na calibração para redefinir a posição lógica atual do ponteiro
  void setCurrentPosition(int32_t steps) override;

  bool isMoving() const override;

  int32_t currentPosition() const override { return _currentPositionSteps; }
  int32_t targetPosition() const override { return _targetPositionSteps; }

private:
  void setDirection(bool forward);
  void pulseStep();

  Config _cfg;

  bool _enabled = false;

  int32_t _currentPositionSteps = 0;
  int32_t _targetPositionSteps = 0;

  float _riseStepsPerSec = 300.0f;
  float _fallStepsPerSec = 300.0f;
  float _activeStepsPerSec = 300.0f;

  uint32_t _lastUpdateMicros = 0;
  float _stepAccumulator = 0.0f;
};
