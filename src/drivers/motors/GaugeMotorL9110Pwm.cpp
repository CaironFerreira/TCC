#include "GaugeMotorL9110Pwm.h"
#include <math.h>

GaugeMotorL9110Pwm::GaugeMotorL9110Pwm(const Config& cfg) : _cfg(cfg) {}

GaugeMotorL9110Pwm::~GaugeMotorL9110Pwm() {
  if (_sinLut) { delete[] _sinLut; _sinLut = nullptr; }
  if (_cosLut) { delete[] _cosLut; _cosLut = nullptr; }
}

void GaugeMotorL9110Pwm::begin(const Config& cfg) {
  _cfg = cfg;
  begin();
}

int GaugeMotorL9110Pwm::clampi(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

int GaugeMotorL9110Pwm::softFloorDuty(float mag, int duty) const {
  // Zona morta: evita tremedeira quando o comando é muito pequeno
  if (mag < _cfg.deadMag) return 0;

  // Piso suave: ajuda a vencer atrito estático sem "trancos"
  if (mag < _cfg.softFloorMag) {
    float denom = (_cfg.softFloorMag - _cfg.deadMag);
    float t = (denom <= 1e-6f) ? 1.0f : (mag - _cfg.deadMag) / denom;

    if (t < 0) t = 0;
    if (t > 1) t = 1;

    // sqrt(t): sobe o duty mais cedo (melhor para evitar "agarra/solta")
    float eased = sqrtf(t);

    int minDuty = (int)lroundf(eased * (float)_cfg.ampFloor);
    if (duty < minDuty) duty = minDuty;
  }
  return duty;
}

void GaugeMotorL9110Pwm::driveCoil(int chPos, int chNeg, float x) {
  if (x > 1.0f) x = 1.0f;
  if (x < -1.0f) x = -1.0f;

  const float mag = fabsf(x);

  int duty = (int)lroundf(mag * (float)_cfg.ampMax);
  duty = clampi(duty, 0, (1 << _cfg.pwmResBits) - 1);
  duty = softFloorDuty(mag, duty);

  if (duty == 0) {
    ledcWrite(chPos, 0);
    ledcWrite(chNeg, 0);
    return;
  }

  if (x >= 0) {
    ledcWrite(chPos, duty);
    ledcWrite(chNeg, 0);
  } else {
    ledcWrite(chPos, 0);
    ledcWrite(chNeg, duty);
  }
}

void GaugeMotorL9110Pwm::begin() {
  if (_cfg.pinA1 < 0 || _cfg.pinA2 < 0 || _cfg.pinB1 < 0 || _cfg.pinB2 < 0) {
    return; // pinos não configurados
  }

  ledcSetup(_cfg.chA1, _cfg.pwmFreq, _cfg.pwmResBits);
  ledcSetup(_cfg.chA2, _cfg.pwmFreq, _cfg.pwmResBits);
  ledcSetup(_cfg.chB1, _cfg.pwmFreq, _cfg.pwmResBits);
  ledcSetup(_cfg.chB2, _cfg.pwmFreq, _cfg.pwmResBits);

  ledcAttachPin(_cfg.pinA1, _cfg.chA1);
  ledcAttachPin(_cfg.pinA2, _cfg.chA2);
  ledcAttachPin(_cfg.pinB1, _cfg.chB1);
  ledcAttachPin(_cfg.pinB2, _cfg.chB2);

  // ===== Pré-computa tabela seno/cosseno =====
  _m = _cfg.microsteps360;
  if (_m < 1) _m = 1;

  if (_sinLut) { delete[] _sinLut; _sinLut = nullptr; }
  if (_cosLut) { delete[] _cosLut; _cosLut = nullptr; }

  _sinLut = new float[_m];
  _cosLut = new float[_m];

  for (int i = 0; i < _m; ++i) {
    float theta = (2.0f * (float)M_PI) * ((float)i / (float)_m);
    _sinLut[i] = sinf(theta);
    _cosLut[i] = cosf(theta);
  }

  _lastStep = 0x7fffffff; // invalida cache
  off();
}

void GaugeMotorL9110Pwm::off() {
  ledcWrite(_cfg.chA1, 0);
  ledcWrite(_cfg.chA2, 0);
  ledcWrite(_cfg.chB1, 0);
  ledcWrite(_cfg.chB2, 0);
}

void GaugeMotorL9110Pwm::applyStep(int step) {
  const int m = _m;
  if (m <= 0) return;

  // Evita reescrever PWM se step não mudou
  if (step == _lastStep) return;
  _lastStep = step;

  int idxMod = step % m;
  if (idxMod < 0) idxMod += m;

  int idxWrap = idxMod;
  if (_cfg.invertDirection) {
    idxWrap = (m - idxMod) % m;
  }

  const float ia = _sinLut ? _sinLut[idxWrap] : 0.0f;
  const float ib = _cosLut ? _cosLut[idxWrap] : 1.0f;

  driveCoil(_cfg.chA1, _cfg.chA2, ia);
  driveCoil(_cfg.chB1, _cfg.chB2, ib);
}