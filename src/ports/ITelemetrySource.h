#pragma once

#include <stdint.h>
#include "domain/TelemetryFrame.h"
#include "domain/TelemetryInputStatus.h"

class ITelemetrySource {
public:
  virtual ~ITelemetrySource() = default;

  virtual bool begin(uint16_t port) = 0;
  virtual void tick() = 0;

  virtual const TelemetryFrame& lastFrame() const = 0;

  virtual bool hasValidTelemetry() const = 0;
  virtual bool hasAnySignal() const = 0;
  virtual TelemetryInputStatus inputStatus() const = 0;
  virtual uint32_t discardedPacketCount() const = 0;
  virtual uint32_t receiveErrorCount() const = 0;

  virtual float speedKmh() const = 0;
  virtual float rpm() const = 0;
  virtual int gear() const = 0;
  virtual int lapNumber() const = 0;
  virtual int racePosition() const = 0;
  virtual float fuelLevel() const = 0;
  virtual float tireTempAvgC() const = 0;
  virtual float tireTempFLC() const = 0;
  virtual float tireTempFRC() const = 0;
  virtual float tireTempRLC() const = 0;
  virtual float tireTempRRC() const = 0;
};
