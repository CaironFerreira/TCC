#pragma once
#include <Arduino.h>

class GaugeMotorTmc2208 {
public:
  struct Config {
    int pinStep = -1;
    int pinDirection  = -1;
    int pinEnable   = -1;

    // Inverte o sentido (caso o ponteiro esteja indo ao contrário)
    bool invertDirection  = false;

    // Pulso STEP em microssegundos (>= 2us costuma funcionar bem)
    uint32_t stepPulseUs = 2;

    // Intervalo MÍNIMO entre passos (quanto maior, mais lento/suave)
    uint32_t stepIntervalUs = 1000;
  };

  explicit GaugeMotorTmc2208(const Config& cfg);

  void begin();
  void enable(bool on);
  bool enabled() const { return _enabled; }

  void setCurrentSteps(int32_t steps);
  int32_t currentSteps() const { return _posSteps; }

  void moveTo(int32_t targetSteps);
  int32_t targetSteps() const { return _targetSteps; }

  bool isMoving() const { return _posSteps != _targetSteps; }

  void tick(uint32_t nowMicros);

private:
  void setDirection(bool forward);
  void pulseStep();

  Config _cfg;

  bool _enabled = false;
  int32_t _posSteps = 0;
  int32_t _targetSteps = 0;

  bool _dirForward = true;
  uint32_t _lastStepMicros = 0;
};