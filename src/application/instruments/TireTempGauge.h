#pragma once
#include <stdint.h>
#include "application/instruments/IInstrument.h"
#include "ports/IClock.h"
#include "ports/IGaugeMotor.h"

class TireTempGauge : public IInstrument {
public:
  struct Config {
    float minTemp = 20.0f;
    float maxTemp = 120.0f;

    int32_t minSteps = 0;
    int32_t maxSteps = 180;

    float normalStepsPerSec = 180.0f;
    float fastStepsPerSec = 220.0f;

    bool invertIndicationDirection = false;
    int32_t calibrationBackoffSteps = 8;
    float calibrationRiseStepsPerSec = 300.0f;
    float calibrationFallStepsPerSec = 300.0f;
  };

  TireTempGauge(IGaugeMotor& motor, IClock& clock, const Config& cfg);

  void begin() override;
  void calibrate();
  void setTemperature(float temp);
  void tick(uint32_t nowMicros) override;
  bool supports(InstrumentSignalType type) const override;
  void apply(const InstrumentSignal& signal) override;

  float currentTemperature() const { return _currentTemp; }
  int32_t targetSteps() const { return _targetSteps; }

private:
  float clampTemp(float temp) const;
  int32_t tempToSteps(float temp) const;

  IGaugeMotor& _motor;
  IClock& _clock;
  Config _cfg;

  float _currentTemp = 20.0f;
  int32_t _targetSteps = 0;
  bool _hasTemperature = false;
};
