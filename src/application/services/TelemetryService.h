#pragma once
#include <stddef.h>
#include <stdint.h>
#include "domain/TelemetryFrame.h"
#include "ports/IClock.h"
#include "ports/IPacketReceiver.h"
#include "ports/ITelemetrySource.h"
#include "ports/ITelemetryDecoder.h"

class TelemetryService : public ITelemetrySource {
public:
  TelemetryService(IPacketReceiver& receiver,
                   ITelemetryDecoder& decoder,
                   IClock& clock);

  bool begin(uint16_t port) override;
  void tick() override;

  const TelemetryFrame& lastFrame() const override { return _last; }

  bool hasValidTelemetry() const override;
  bool hasAnySignal() const override { return _hasAnySignal; }
  TelemetryInputStatus inputStatus() const override { return _inputStatus; }
  uint32_t discardedPacketCount() const override { return _discardedPacketCount; }
  uint32_t receiveErrorCount() const override { return _receiveErrorCount; }

  float speedKmh() const override;
  float rpm() const override;
  int gear() const override;
  int lapNumber() const override;
  int racePosition() const override;
  float fuelLevel() const override;
  float tireTempAvgC() const override;
  float tireTempFLC() const override;
  float tireTempFRC() const override;
  float tireTempRLC() const override;
  float tireTempRRC() const override;

private:
  IPacketReceiver& _receiver;
  ITelemetryDecoder& _decoder;
  IClock& _clock;

  TelemetryFrame _last;
  uint32_t _lastPacketMs = 0;
  uint32_t _lastAnyPacketMs = 0;
  bool _hasAnySignal = false;
  TelemetryInputStatus _inputStatus = TelemetryInputStatus::Idle;
  uint32_t _discardedPacketCount = 0;
  uint32_t _receiveErrorCount = 0;

  static constexpr size_t BUF_SIZE = 512;
  uint8_t _buf[BUF_SIZE];

  static constexpr uint8_t PACKET_DRAIN_LIMIT = 64;
  static constexpr uint32_t SIGNAL_TIMEOUT_MS = 300;
};
