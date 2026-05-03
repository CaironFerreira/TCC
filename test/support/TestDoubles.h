#pragma once

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "application/instruments/IInstrument.h"
#include "domain/UiStatus.h"
#include "domain/TelemetryFrame.h"
#include "ports/IButtonInput.h"
#include "ports/IClock.h"
#include "ports/IGaugeMotor.h"
#include "ports/IInstrumentCluster.h"
#include "ports/IPacketReceiver.h"
#include "ports/IStatusDisplay.h"
#include "ports/ITelemetryDecoder.h"
#include "ports/ITelemetrySource.h"
#include "ports/IWifiConfigPortal.h"

namespace test_support {

static inline void copyText(char* target, size_t targetSize, const char* value) {
  if (target == nullptr || targetSize == 0) {
    return;
  }

  if (value == nullptr) {
    target[0] = '\0';
    return;
  }

  strncpy(target, value, targetSize - 1);
  target[targetSize - 1] = '\0';
}

class FakeClock : public IClock {
public:
  uint32_t ms = 0;
  uint32_t micros = 0;

  uint32_t nowMs() const override { return ms; }
  uint32_t nowMicros() const override { return micros; }

  void advanceMs(uint32_t deltaMs) {
    ms += deltaMs;
    micros += deltaMs * 1000U;
  }

  void setMs(uint32_t value) {
    ms = value;
    micros = value * 1000U;
  }

  void setMicros(uint32_t value) {
    micros = value;
    ms = value / 1000U;
  }
};

class FakeGaugeMotor : public IGaugeMotor {
public:
  int beginCalls = 0;
  int enableCalls = 0;
  int tickCalls = 0;
  int moveCalls = 0;
  int setPositionCalls = 0;
  bool lastEnableValue = false;
  uint32_t lastTickMicros = 0;
  float lastRiseStepsPerSec = 0.0f;
  float lastFallStepsPerSec = 0.0f;
  int32_t currentPos = 0;
  int32_t targetPos = 0;
  bool moving = false;
  bool autoStopOnTick = true;

  void begin() override { ++beginCalls; }

  void enable(bool on) override {
    ++enableCalls;
    lastEnableValue = on;
  }

  void tick(uint32_t nowMicros) override {
    ++tickCalls;
    lastTickMicros = nowMicros;
    if (moving && autoStopOnTick) {
      moving = false;
      currentPos = targetPos;
    }
  }

  void moveTo(int32_t targetSteps,
              float riseStepsPerSec,
              float fallStepsPerSec) override {
    ++moveCalls;
    targetPos = targetSteps;
    lastRiseStepsPerSec = riseStepsPerSec;
    lastFallStepsPerSec = fallStepsPerSec;
    moving = autoStopOnTick;
  }

  void setCurrentPosition(int32_t steps) override {
    ++setPositionCalls;
    currentPos = steps;
  }

  bool isMoving() const override { return moving; }
  int32_t currentPosition() const override { return currentPos; }
  int32_t targetPosition() const override { return targetPos; }
};

class FakePacketReceiver : public IPacketReceiver {
public:
  static constexpr size_t MAX_EVENTS = 16;
  static constexpr size_t MAX_PACKET_BYTES = 512;

  struct Event {
    PacketReceiveStatus status = PacketReceiveStatus::NoPacket;
    size_t size = 0;
    uint8_t data[MAX_PACKET_BYTES] = {0};
  };

  bool beginResult = true;
  bool beginCalled = false;
  uint16_t begunPort = 0;
  size_t receiveCalls = 0;

  bool begin(uint16_t port) override {
    beginCalled = true;
    begunPort = port;
    return beginResult;
  }

  PacketReceiveResult receive(uint8_t* buffer, size_t bufferSize) override {
    ++receiveCalls;

    if (_head >= _count) {
      return PacketReceiveResult{};
    }

    const Event& event = _events[_head++];
    PacketReceiveResult result;
    result.status = event.status;
    result.bytesRead = event.size;

    if (event.status == PacketReceiveStatus::PacketReceived &&
        buffer != nullptr &&
        bufferSize >= event.size) {
      memcpy(buffer, event.data, event.size);
    }

    return result;
  }

  void enqueuePacket(const uint8_t* data, size_t size) {
    Event event;
    event.status = PacketReceiveStatus::PacketReceived;
    event.size = size;
    if (data != nullptr && size > 0) {
      memcpy(event.data, data, size);
    }
    push(event);
  }

  void enqueueTooLarge() {
    Event event;
    event.status = PacketReceiveStatus::PacketTooLarge;
    push(event);
  }

  void enqueueReceiveError() {
    Event event;
    event.status = PacketReceiveStatus::ReceiveError;
    push(event);
  }

  void enqueueNoPacket() {
    Event event;
    event.status = PacketReceiveStatus::NoPacket;
    push(event);
  }

private:
  Event _events[MAX_EVENTS] = {};
  size_t _count = 0;
  size_t _head = 0;

  void push(const Event& event) {
    if (_count < MAX_EVENTS) {
      _events[_count++] = event;
    }
  }
};

class FakeTelemetryDecoder : public ITelemetryDecoder {
public:
  bool decodeResult = true;
  int decodeCalls = 0;
  size_t lastLen = 0;
  TelemetryFrame frameToReturn{};

  bool decode(const uint8_t* data, size_t len, TelemetryFrame& out) override {
    (void)data;
    ++decodeCalls;
    lastLen = len;
    if (decodeResult) {
      out = frameToReturn;
      return true;
    }

    out.invalidate();
    return false;
  }

  const char* name() const override { return "Fake decoder"; }
};

class FakeTelemetrySource : public ITelemetrySource {
public:
  bool beginResult = true;
  bool beginCalled = false;
  int tickCalls = 0;
  uint16_t begunPort = 0;

  TelemetryFrame frame{};
  bool validTelemetry = false;
  bool anySignal = false;
  TelemetryInputStatus status = TelemetryInputStatus::Idle;
  uint32_t discardedPackets = 0;
  uint32_t receiveErrors = 0;
  float speed = 0.0f;
  float rpmValue = 0.0f;
  int gearValue = 0;
  int lapValue = 0;
  int positionValue = 0;
  float fuelValue = 0.0f;
  float tempAvgValue = 0.0f;
  float tempFlValue = 0.0f;
  float tempFrValue = 0.0f;
  float tempRlValue = 0.0f;
  float tempRrValue = 0.0f;

  bool begin(uint16_t port) override {
    beginCalled = true;
    begunPort = port;
    return beginResult;
  }

  void tick() override { ++tickCalls; }

  const TelemetryFrame& lastFrame() const override { return frame; }
  bool hasValidTelemetry() const override { return validTelemetry; }
  bool hasAnySignal() const override { return anySignal; }
  TelemetryInputStatus inputStatus() const override { return status; }
  uint32_t discardedPacketCount() const override { return discardedPackets; }
  uint32_t receiveErrorCount() const override { return receiveErrors; }
  float speedKmh() const override { return speed; }
  float rpm() const override { return rpmValue; }
  int gear() const override { return gearValue; }
  int lapNumber() const override { return lapValue; }
  int racePosition() const override { return positionValue; }
  float fuelLevel() const override { return fuelValue; }
  float tireTempAvgC() const override { return tempAvgValue; }
  float tireTempFLC() const override { return tempFlValue; }
  float tireTempFRC() const override { return tempFrValue; }
  float tireTempRLC() const override { return tempRlValue; }
  float tireTempRRC() const override { return tempRrValue; }
};

class FakeStatusDisplay : public IStatusDisplay {
public:
  static constexpr size_t LINE_CAPACITY = 64;

  bool beginResult = true;
  bool nextLayoutResult = false;
  int beginCalls = 0;
  int nextLayoutCalls = 0;
  int setStatusCalls = 0;
  int showMessageCalls = 0;
  int tickCalls = 0;
  uint8_t lastLayoutId = 0;
  UiStatus lastStatus{};
  char lastMessage1[LINE_CAPACITY] = "";
  char lastMessage2[LINE_CAPACITY] = "";
  char lastMessage3[LINE_CAPACITY] = "";
  char lastMessage4[LINE_CAPACITY] = "";

  bool begin(uint8_t layoutId) override {
    ++beginCalls;
    lastLayoutId = layoutId;
    return beginResult;
  }

  bool nextLayout() override {
    ++nextLayoutCalls;
    return nextLayoutResult;
  }

  void setStatus(const UiStatus& status) override {
    ++setStatusCalls;
    lastStatus = status;
  }

  void showMessage(const char* line1,
                   const char* line2,
                   const char* line3,
                   const char* line4 = nullptr) override {
    ++showMessageCalls;
    copyText(lastMessage1, sizeof(lastMessage1), line1);
    copyText(lastMessage2, sizeof(lastMessage2), line2);
    copyText(lastMessage3, sizeof(lastMessage3), line3);
    copyText(lastMessage4, sizeof(lastMessage4), line4);
  }

  void tick() override { ++tickCalls; }
};

class FakeInstrumentCluster : public IInstrumentCluster {
public:
  int beginCalls = 0;
  int speedCalls = 0;
  int rpmCalls = 0;
  int fuelCalls = 0;
  int tireTempCalls = 0;
  int tickCalls = 0;
  float lastSpeed = NAN;
  float lastRpm = NAN;
  float lastFuel = NAN;
  float lastTireTemp = NAN;
  uint32_t lastTickMicros = 0;

  void begin() override { ++beginCalls; }

  void setSpeedKmh(float speedKmh) override {
    ++speedCalls;
    lastSpeed = speedKmh;
  }

  void setRpm(float rpm) override {
    ++rpmCalls;
    lastRpm = rpm;
  }

  void setFuelLevel(float fuelLevel) override {
    ++fuelCalls;
    lastFuel = fuelLevel;
  }

  void setTireTemperature(float temperatureC) override {
    ++tireTempCalls;
    lastTireTemp = temperatureC;
  }

  void tick(uint32_t nowMicros) override {
    ++tickCalls;
    lastTickMicros = nowMicros;
  }
};

class FakeButtonInput : public IButtonInput {
public:
  bool pressed = false;
  int beginCalls = 0;
  int tickCalls = 0;
  uint32_t lastTickMs = 0;

  void begin() override { ++beginCalls; }

  void tick(uint32_t nowMs) override {
    ++tickCalls;
    lastTickMs = nowMs;
  }

  bool wasPressed() override {
    const bool result = pressed;
    pressed = false;
    return result;
  }
};

class FakeWifiConfigPortal : public IWifiConfigPortal {
public:
  bool connected = false;
  bool portalActive = false;
  int beginCalls = 0;
  int tickCalls = 0;
  char ssid[UiStatus::SSID_CAPACITY] = "SimHub";
  char ip[UiStatus::IP_CAPACITY] = "192.168.0.10";

  void begin() override {
    ++beginCalls;
  }

  void tick() override { ++tickCalls; }
  bool isConnected() const override { return connected; }
  bool isPortalActive() const override { return portalActive; }

  void copyLocalIp(char* buffer, size_t bufferSize) const override {
    copyText(buffer, bufferSize, ip);
  }

  void copyConnectedSsid(char* buffer, size_t bufferSize) const override {
    copyText(buffer, bufferSize, ssid);
  }
};

class FakeInstrument : public IInstrument {
public:
  static constexpr size_t MAX_SIGNALS = 8;

  InstrumentSignalType supportedType = InstrumentSignalType::SpeedKmh;
  int beginCalls = 0;
  int tickCalls = 0;
  int applyCalls = 0;
  InstrumentSignal receivedSignals[MAX_SIGNALS] = {};
  size_t receivedCount = 0;
  uint32_t lastTickMicros = 0;

  void begin() override { ++beginCalls; }

  void tick(uint32_t nowMicros) override {
    ++tickCalls;
    lastTickMicros = nowMicros;
  }

  bool supports(InstrumentSignalType type) const override {
    return type == supportedType;
  }

  void apply(const InstrumentSignal& signal) override {
    ++applyCalls;
    if (receivedCount < MAX_SIGNALS) {
      receivedSignals[receivedCount++] = signal;
    }
  }
};

}  // namespace test_support
