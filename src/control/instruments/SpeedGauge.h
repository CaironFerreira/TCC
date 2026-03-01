#pragma once
#include <Arduino.h>
#include "drivers/motors/GaugeMotorL9110Pwm.h"

class SpeedGauge {
public:
  struct Config {
    // Escala
    float speedMaxKmh = 220.0f;
    int   rangeSteps0ToMax = 3450;

    // Dinâmica (limites)
    float maxStepsPerS = 4000.0f;
    float maxAccelStepsPerS2 = 30000.0f;

    // Controle: v = kpVel * erro
    float kpVel = 45.0f;             // (passos/s) por passo de erro

    // Suavização de alvo (telemetria ruidosa)
    float targetFilterHz = 6.0f;     // 0 = sem filtro; 4–10 Hz costuma funcionar bem

    // Zona morta / snap (reduz jitter perto do alvo)
    float deadbandSteps = 1.2f;      // abaixo disso não fica “microcorrigindo”
    float snapSteps = 0.6f;          // abaixo disso cola no alvo

    // Segurança de dt
    float maxDtS = 0.03f;            // 30 ms (se travar, não dá salto grande)

    // Homing (batente)
    uint32_t homeTimeMs  = 1200;
    uint32_t homeStepMs  = 2;
    int homeStepInc      = 6;
    int homeDirSign      = -1;
  };

  SpeedGauge(GaugeMotorL9110Pwm& motor, const Config& cfg);

  void begin(const Config& cfg);
  void homeToStop();

  void setSpeedKmh(float kmh);
  void update();

private:
  GaugeMotorL9110Pwm& _motor;
  Config _cfg;

  float _pos = 0.0f;         // posição estimada (passos)
  float _vel = 0.0f;         // velocidade (passos/s)

  float _targetRaw = 0.0f;   // alvo direto (passos)
  float _targetF = 0.0f;     // alvo filtrado (passos)

  uint32_t _lastUs = 0;
  int _lastAppliedStep = INT32_MIN;

  inline float maxPos() const {
    return (_cfg.rangeSteps0ToMax > 1) ? (float)(_cfg.rangeSteps0ToMax - 1) : 0.0f;
  }
};