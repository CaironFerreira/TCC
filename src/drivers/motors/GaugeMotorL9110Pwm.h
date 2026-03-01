#pragma once
#include <Arduino.h>

class GaugeMotorL9110Pwm {
public:
  struct Config {
    // Pinos ESP32 -> L9110
    int pinA1 = -1;
    int pinA2 = -1;
    int pinB1 = -1;
    int pinB2 = -1;

    // LEDC
    uint32_t pwmFreq = 20000;
    uint8_t  pwmResBits = 8;

    // “microsteps” virtuais por volta (tabela seno/cosseno)
    int microsteps360 = 3450;

    // Amplitudes (0..(2^pwmResBits - 1))
    int ampMax   = 200;
    int ampFloor = 18;

    // Zonas mortas / soft floor
    float deadMag      = 0.02f;
    float softFloorMag = 0.15f;

    // Direção
    bool invertDirection = false;

    // Canais LEDC (fixos)
    int chA1 = 0;
    int chA2 = 1;
    int chB1 = 2;
    int chB2 = 3;
  };

  explicit GaugeMotorL9110Pwm(const Config& cfg);

  // Padrão do seu AppConfig: aplica cfg no begin()
  void begin(const Config& cfg);
  void begin();

  void off();
  void applyStep(int step);

  // Importante para liberar LUTs se você recriar o objeto (ou reinicializar)
  ~GaugeMotorL9110Pwm();

private:
  Config _cfg;

  int clampi(int v, int lo, int hi);
  int softFloorDuty(float mag, int duty) const;
  void driveCoil(int chPos, int chNeg, float x);

  // ===== Melhorias para suavidade/consistência =====
  int _m = 0;                      // cache de microsteps360
  float* _sinLut = nullptr;        // LUT seno
  float* _cosLut = nullptr;        // LUT cosseno
  int _lastStep = 0x7fffffff;      // evita reescrever PWM se step não mudou
};
