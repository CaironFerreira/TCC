#include "application/services/InstrumentCluster.h"

InstrumentCluster::InstrumentCluster(IInstrument* const* instruments, size_t instrumentCount)
  : _instruments(instruments), _instrumentCount(instrumentCount) {}

void InstrumentCluster::begin() {
  for (size_t i = 0; i < _instrumentCount; ++i) {
    if (_instruments[i] != nullptr) {
      _instruments[i]->begin();
    }
  }
}

void InstrumentCluster::dispatchSignal(InstrumentSignalType type, float value) {
  InstrumentSignal signal;
  signal.type = type;
  signal.value = value;

  for (size_t i = 0; i < _instrumentCount; ++i) {
    IInstrument* instrument = _instruments[i];
    if (instrument != nullptr && instrument->supports(type)) {
      instrument->apply(signal);
    }
  }
}

void InstrumentCluster::setSpeedKmh(float speedKmh) {
  dispatchSignal(InstrumentSignalType::SpeedKmh, speedKmh);
}

void InstrumentCluster::setRpm(float rpm) {
  dispatchSignal(InstrumentSignalType::Rpm, rpm);
}

void InstrumentCluster::setFuelLevel(float fuelLevel) {
  dispatchSignal(InstrumentSignalType::FuelLevel, fuelLevel);
}

void InstrumentCluster::setTireTemperature(float temperatureC) {
  dispatchSignal(InstrumentSignalType::TireTemperatureC, temperatureC);
}

void InstrumentCluster::tick(uint32_t nowMicros) {
  for (size_t i = 0; i < _instrumentCount; ++i) {
    if (_instruments[i] != nullptr) {
      _instruments[i]->tick(nowMicros);
    }
  }
}
