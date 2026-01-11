#include "StepperL9110.h"
#include <math.h>

StepperL9110::StepperL9110(const Pins& pins, int chA1, int chA2, int chB1, int chB2)
  : _pins(pins), _chA1(chA1), _chA2(chA2), _chB1(chB1), _chB2(chB2) {}

void StepperL9110::begin(const Config& cfg) {
  _cfg = cfg;
  _q = _cfg.q;
  _cycle = 4 * _q;

  // LEDC
  ledcSetup(_chA1, _cfg.pwmFreq, _cfg.pwmResBits);
  ledcSetup(_chA2, _cfg.pwmFreq, _cfg.pwmResBits);
  ledcSetup(_chB1, _cfg.pwmFreq, _cfg.pwmResBits);
  ledcSetup(_chB2, _cfg.pwmFreq, _cfg.pwmResBits);

  ledcAttachPin(_pins.a1, _chA1);
  ledcAttachPin(_pins.a2, _chA2);
  ledcAttachPin(_pins.b1, _chB1);
  ledcAttachPin(_pins.b2, _chB2);

  buildSinTable();
  applyMicrostep(_phase);

  _lastStepUs = micros();
}

void StepperL9110::buildSinTable() {
  // libera anterior se houver
  if (_sinQ) { delete[] _sinQ; _sinQ = nullptr; }
  _sinQ = new uint8_t[_q + 1];

  for (int i = 0; i <= _q; i++) {
    float ang = (M_PI * 0.5f) * ((float)i / (float)_q);
    int v = (int)lroundf(sinf(ang) * 255.0f);
    if (v < 0) v = 0;
    if (v > 255) v = 255;
    _sinQ[i] = (uint8_t)v;
  }
}

void StepperL9110::driveCoil(int chPlus, int chMinus, int valueSigned) {
  int v = abs(valueSigned);

  // piso só quando v > 0
  if (v > 0 && v < _cfg.floor) v = _cfg.floor;

  if (valueSigned >= 0) {
    pwmWrite(chPlus, v);
    pwmWrite(chMinus, 0);
  } else {
    pwmWrite(chPlus, 0);
    pwmWrite(chMinus, v);
  }
}

void StepperL9110::applyMicrostep(int ph) {
  ph %= _cycle;
  if (ph < 0) ph += _cycle;

  int q = ph / _q;  // 0..3
  int i = ph % _q;  // 0..Q-1

  int s = _sinQ[i];
  int c = _sinQ[_q - i];

  int sinv, cosv;
  switch (q) {
    case 0:  sinv =  s; cosv =  c; break;
    case 1:  sinv =  c; cosv = -s; break;
    case 2:  sinv = -s; cosv = -c; break;
    default: sinv = -c; cosv =  s; break;
  }

  sinv = (sinv * _cfg.amp) / 255;
  cosv = (cosv * _cfg.amp) / 255;

  driveCoil(_chA1, _chA2, sinv);
  driveCoil(_chB1, _chB2, cosv);
}

void StepperL9110::setTarget(int targetMicrosteps) {
  _target = targetMicrosteps;
}

void StepperL9110::setPosition(int posMicrosteps) {
  _pos = posMicrosteps;
}

void StepperL9110::tick() {
  if (_pos == _target) return;

  uint32_t now = micros();
  if ((uint32_t)(now - _lastStepUs) < (uint32_t)_cfg.stepUs) return;
  _lastStepUs = now;

  // direção lógica (posição)
  int dirLogical = (_target > _pos) ? +1 : -1;

  // direção física (motor)
  int dirPhysical = dirLogical;
  if (_cfg.invertDir) {
    dirPhysical = -dirPhysical;
  }

  // move fisicamente o motor
  _phase += dirPhysical;
  applyMicrostep(_phase);

  // atualiza posição lógica (SEMPRE no sentido correto)
  _pos += dirLogical;
}

void StepperL9110::homeByHardStop(int fullScaleMicrosteps) {
  int dirPhysical = -1;
  if (_cfg.invertDir) dirPhysical = +1;

  for (int i = 0; i < fullScaleMicrosteps; i++) {
    _phase += dirPhysical;
    applyMicrostep(_phase);
    delayMicroseconds(_cfg.stepUs);
  }

  // posição lógica zerada corretamente
  _pos = 0;
  _target = 0;
}

