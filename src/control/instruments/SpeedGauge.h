#pragma once
#include <Arduino.h>

#include "drivers/motors/GaugeMotorTmc2208.h"

class SpeedGauge {
public:
  struct Config {
    float maxSpeedKmh = 240.0f;   // velocidade máxima da escala
    int32_t minSteps = 0;         // posição do ponteiro em 0 km/h
    int32_t maxSteps = 180;     // posição do ponteiro na velocidade máxima
  };

  SpeedGauge(GaugeMotorTmc2208& motor, const Config& cfg);

  void begin();

  // Atualiza a velocidade do instrumento
  void setSpeedKmh(float speedKmh);

  // Chamado no loop principal
  void tick(uint32_t nowMicros);

  float currentSpeedKmh() const { return _currentSpeedKmh; }
  int32_t targetSteps() const { return _targetSteps; }

private:
  int32_t speedToSteps(float speedKmh) const;
  float clampSpeed(float speedKmh) const;

  GaugeMotorTmc2208& _motor;
  Config _cfg;

  float _currentSpeedKmh = 0.0f;
  int32_t _targetSteps = 0;
};