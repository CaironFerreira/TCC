#pragma once
#include <stdint.h>

#include "application/instruments/IInstrument.h"
#include "ports/IClock.h"
#include "ports/IGaugeMotor.h"

class RpmGauge : public IInstrument {
public:
  struct Config {
    float maxRpm = 8000.0f;
    int32_t minSteps = 0;
    int32_t maxSteps = 180;

    float normalStepsPerSec = 320.0f;
    float fastStepsPerSec = 420.0f;

    float calibrationRiseStepsPerSec = 300.0f;
    float calibrationFallStepsPerSec = 300.0f;
  };

  RpmGauge(IGaugeMotor& motor, IClock& clock, const Config& cfg);

  void begin() override;
  void calibrate();
  void setRpm(float rpm);
  void tick(uint32_t nowMicros) override;
  bool supports(InstrumentSignalType type) const override;
  void apply(const InstrumentSignal& signal) override;

  float currentRpm() const { return _currentRpm; }
  int32_t targetSteps() const { return _targetSteps; }

private:
  int32_t rpmToSteps(float rpm) const;
  float clampRpm(float rpm) const;

  IGaugeMotor& _motor;
  IClock& _clock;
  Config _cfg;

  float _currentRpm = 0.0f;
  int32_t _targetSteps = 0;
};
