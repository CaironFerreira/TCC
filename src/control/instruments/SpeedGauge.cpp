#include "SpeedGauge.h"

SpeedGauge::SpeedGauge(StepperL9110& motor, int fullScaleMicrosteps)
  : _m(motor), _fullScale(fullScaleMicrosteps) {}

void SpeedGauge::begin() {}

int SpeedGauge::speedToSteps(float kmh) const {
  if (kmh < 0.0f) kmh = 0.0f;
  if (kmh > SPEED_MAX_KMH) kmh = SPEED_MAX_KMH;

  // map 0..220 -> 0..fullScale
  float ratio = kmh / SPEED_MAX_KMH;
  int steps = (int)(ratio * (float)_fullScale + 0.5f);
  if (steps < 0) steps = 0;
  if (steps > _fullScale) steps = _fullScale;
  return steps;
}

void SpeedGauge::update(const TelemetryFrame& f) {
  if (!f.valid) {
    _m.setTarget(0);
    return;
  }
  _m.setTarget(speedToSteps(f.speedKmh));
}
