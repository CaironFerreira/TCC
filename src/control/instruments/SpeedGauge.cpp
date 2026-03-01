#include "SpeedGauge.h"
#include <math.h>

static inline float clampf(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

static inline float expSmoothingAlpha(float cutoffHz, float dt) {
  // alpha = 1 - exp(-2*pi*fc*dt)
  if (cutoffHz <= 0.0f || dt <= 0.0f) return 1.0f; // sem filtro
  const float w = 2.0f * (float)M_PI * cutoffHz;
  return 1.0f - expf(-w * dt);
}

SpeedGauge::SpeedGauge(GaugeMotorL9110Pwm& motor, const Config& cfg)
  : _motor(motor), _cfg(cfg) {}

void SpeedGauge::begin(const Config& cfg) {
  _cfg = cfg;

  _pos = 0.0f;
  _vel = 0.0f;

  _targetRaw = 0.0f;
  _targetF   = 0.0f;

  _lastUs = 0;
  _lastAppliedStep = INT32_MIN;

  // garante estado conhecido no driver
  _motor.applyStep(0);
  _lastAppliedStep = 0;
}

void SpeedGauge::homeToStop() {
  // começa no meio do curso para garantir empurrar até o batente
  int idx = _cfg.rangeSteps0ToMax / 2;
  idx = (int)clampf((float)idx, 0.0f, maxPos());
  _motor.applyStep(idx);
  delay(60);

  const uint32_t start = millis();
  while ((millis() - start) < _cfg.homeTimeMs) {
    idx += _cfg.homeDirSign * _cfg.homeStepInc;
    // durante homing, não precisa clampar agressivo: o batente é físico
    _motor.applyStep(idx);
    delay(_cfg.homeStepMs);
  }

  // batente = zero lógico
  _pos = 0.0f;
  _vel = 0.0f;
  _targetRaw = 0.0f;
  _targetF   = 0.0f;

  _lastUs = 0;
  _motor.applyStep(0);
  _lastAppliedStep = 0;
}

void SpeedGauge::setSpeedKmh(float kmh) {
  if (!isfinite(kmh)) return;

  if (kmh < 0.0f) kmh = 0.0f;
  if (kmh > _cfg.speedMaxKmh) kmh = _cfg.speedMaxKmh;

  const float t = (_cfg.speedMaxKmh <= 0.0f) ? 0.0f : (kmh / _cfg.speedMaxKmh);
  _targetRaw = clampf(t * maxPos(), 0.0f, maxPos());
}

void SpeedGauge::update() {
  const uint32_t nowUs = micros();

  if (_lastUs == 0) {
    _lastUs = nowUs;
    const int step = (int)lroundf(_pos);
    _motor.applyStep(step);
    _lastAppliedStep = step;
    return;
  }

  float dt = (nowUs - _lastUs) * 1e-6f;
  _lastUs = nowUs;

  if (dt <= 0.0f) return;
  if (_cfg.maxDtS > 0.0f && dt > _cfg.maxDtS) dt = _cfg.maxDtS;

  // 1) Filtra o alvo para matar jitter da telemetria
  const float alpha = expSmoothingAlpha(_cfg.targetFilterHz, dt);
  _targetF = _targetF + alpha * (_targetRaw - _targetF);

  // 2) Erro em passos (posição conhecida do ponteiro)
  const float err = _targetF - _pos;
  const float aerr = fabsf(err);

  // 3) Deadband / snap para evitar caça perto do alvo
  if (aerr <= _cfg.snapSteps) {
    _pos = _targetF;
    _vel = 0.0f;
  } else if (aerr <= _cfg.deadbandSteps) {
    // dentro da deadband, não acelera: só deixa chegar por inércia e freia
    _vel *= 0.85f;
  } else {
    // 4) Lei principal: velocidade proporcional ao erro
    float vDes = err * _cfg.kpVel;

    // limita velocidade
    vDes = clampf(vDes, -_cfg.maxStepsPerS, _cfg.maxStepsPerS);

    // 5) limita aceleração (suavidade)
    const float dvMax = _cfg.maxAccelStepsPerS2 * dt;
    float dv = vDes - _vel;
    dv = clampf(dv, -dvMax, dvMax);
    _vel += dv;

    // 6) integra posição
    _pos += _vel * dt;
  }

  // clamp final no range
  _pos = clampf(_pos, 0.0f, maxPos());

  // 7) aplica no motor apenas se o passo mudou (reduz “buzz”)
  const int step = (int)lroundf(_pos);
  if (step != _lastAppliedStep) {
    _motor.applyStep(step);
    _lastAppliedStep = step;
  }
}