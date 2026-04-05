#pragma once
#include <Arduino.h>

class GaugeMotorTmc2208 {
public:
  struct Config {
    int pinStep = -1;
    int pinDirection = -1;
    int pinEnable = -1;
    bool invertDirection = false;
    uint32_t stepPulseUs = 2;
  };

  explicit GaugeMotorTmc2208(const Config& cfg);

  void begin();
  void enable(bool on);
  void tick(uint32_t nowMicros);

  // Nova arquitetura: cada comando já informa as velocidades de subida/descida
  void moveTo(int32_t targetSteps, float riseStepsPerSec, float fallStepsPerSec);

  // Usado na calibração para redefinir a posição lógica atual do ponteiro
  void setCurrentPosition(int32_t steps);

  bool isMoving() const;

  int32_t currentPosition() const { return _currentPositionSteps; }
  int32_t targetPosition() const { return _targetPositionSteps; }

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