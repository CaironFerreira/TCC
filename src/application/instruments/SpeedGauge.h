#pragma once
#include <stdint.h>
#include "application/instruments/IInstrument.h"
#include "ports/IClock.h"
#include "ports/IGaugeMotor.h"

class SpeedGauge : public IInstrument {
public:
  struct Config {
    float maxSpeedKmh = 220.0f;
    int32_t minSteps = 0;
    int32_t maxSteps = 180;
    float normalStepsPerSec = 60.0f;
    float fastStepsPerSec = 800.0f;

    // ===== Calibração própria do velocímetro =====
    int32_t calibrationBackoffSteps = 8;
    float calibrationRiseStepsPerSec = 300.0f;
    float calibrationFallStepsPerSec = 300.0f;
  };

  SpeedGauge(IGaugeMotor& motor, IClock& clock, const Config& cfg);

  void begin() override;
  void calibrate();
  void setSpeedKmh(float speedKmh);
  void tick(uint32_t nowMicros) override;
  bool supports(InstrumentSignalType type) const override;
  void apply(const InstrumentSignal& signal) override;

private:
  float clampSpeed(float speedKmh) const;
  int32_t speedToSteps(float speedKmh) const;

  IGaugeMotor& _motor;
  IClock& _clock;
  Config _cfg;

  float _currentSpeedKmh = 0.0f;
  int32_t _targetSteps = 0;
};
