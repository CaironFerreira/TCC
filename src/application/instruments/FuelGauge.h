#pragma once
#include <stdint.h>
#include "application/instruments/IInstrument.h"
#include "ports/IClock.h"
#include "ports/IGaugeMotor.h"

class FuelGauge : public IInstrument {
public:
  struct Config {
    float minFuelLevel = 0.0f;
    float maxFuelLevel = 1.0f;

    int32_t minSteps = 0;
    int32_t maxSteps = 180;

    float normalStepsPerSec = 120.0f;
    float fastStepsPerSec = 120.0f;

    float calibrationRiseStepsPerSec = 300.0f;
    float calibrationFallStepsPerSec = 300.0f;
  };

  FuelGauge(IGaugeMotor& motor, IClock& clock, const Config& cfg);

  void begin() override;
  void calibrate();
  void setFuelLevel(float level);
  void tick(uint32_t nowMicros) override;
  bool supports(InstrumentSignalType type) const override;
  void apply(const InstrumentSignal& signal) override;

  float currentFuelLevel() const { return _currentFuelLevel; }
  int32_t targetSteps() const { return _targetSteps; }

private:
  float clampFuelLevel(float level) const;
  int32_t fuelToSteps(float level) const;

  IGaugeMotor& _motor;
  IClock& _clock;
  Config _cfg;

  float _currentFuelLevel = 0.0f;
  int32_t _targetSteps = 0;
  bool _hasFuelLevel = false;
};
