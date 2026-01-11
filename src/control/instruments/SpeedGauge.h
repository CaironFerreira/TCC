#pragma once
#include "../../domain/TelemetryFrame.h"
#include "../../drivers/motors/StepperL9110.h"

class SpeedGauge {
public:
  SpeedGauge(StepperL9110& motor, int fullScaleMicrosteps);

  void begin(); // opcional
  void update(const TelemetryFrame& f);

private:
  StepperL9110& _m;
  const int _fullScale;

  static constexpr float SPEED_MAX_KMH = 220.0f;

  int speedToSteps(float kmh) const;
};
