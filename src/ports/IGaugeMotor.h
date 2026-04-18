#pragma once

#include <stdint.h>

class IGaugeMotor {
public:
  virtual ~IGaugeMotor() = default;

  virtual void begin() = 0;
  virtual void enable(bool on) = 0;
  virtual void tick(uint32_t nowMicros) = 0;
  virtual void moveTo(int32_t targetSteps,
                      float riseStepsPerSec,
                      float fallStepsPerSec) = 0;
  virtual void setCurrentPosition(int32_t steps) = 0;
  virtual bool isMoving() const = 0;
  virtual int32_t currentPosition() const = 0;
  virtual int32_t targetPosition() const = 0;
};
