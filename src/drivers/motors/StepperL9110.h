#pragma once
#include <Arduino.h>

class StepperL9110 {
public:
  struct Pins {
    int a1, a2, b1, b2;
  };

  struct Config {
    int pwmFreq = 20000;
    int pwmResBits = 8;     // 0..255
    int amp = 200;          // 150..230
    int floor = 18;         // 10..35
    int stepUs = 2000;      // 800..2000
    int q = 32;             // microsteps por quadrante

    bool invertDir = false; // <<< CONTROLE DE SENTIDO
  };

  StepperL9110(const Pins& pins, int chA1, int chA2, int chB1, int chB2);

  void begin(const Config& cfg);

  // Homing simples por batente: anda "para esquerda" um curso inteiro
  // e considera posição 0 ao final.
  void homeByHardStop(int fullScaleMicrosteps);

  // Controle por alvo (não bloqueante)
  void setTarget(int targetMicrosteps);
  int  target() const { return _target; }
  int  position() const { return _pos; }

  // chama frequentemente no loop; move 1 microstep por tick (se necessário)
  void tick();

  // opcional: força posição
  void setPosition(int posMicrosteps);

private:
  void buildSinTable();
  void applyMicrostep(int ph);
  void driveCoil(int chPlus, int chMinus, int valueSigned);

  inline void pwmWrite(int ch, int duty) { ledcWrite(ch, duty); }

private:
  Pins _pins;
  Config _cfg;

  int _chA1, _chA2, _chB1, _chB2;

  int _phase = 0;     // fase elétrica (0..CYCLE-1)
  int _pos = 0;       // posição mecânica em microsteps (0..fullScale)
  int _target = 0;

  int _q = 32;
  int _cycle = 128;

  uint8_t* _sinQ = nullptr; // tabela 0..Q

  uint32_t _lastStepUs = 0;
};
