#include "drivers/input/GpioButtonInput.h"

GpioButtonInput::GpioButtonInput(const Config& cfg)
: _cfg(cfg) {}

void GpioButtonInput::begin() {
  if (_cfg.pin < 0) {
    return;
  }

  if (_cfg.useInternalPullup) {
    pinMode(_cfg.pin, INPUT_PULLUP);
  } else if (_cfg.useInternalPulldown) {
    pinMode(_cfg.pin, INPUT_PULLDOWN);
  } else {
    pinMode(_cfg.pin, INPUT);
  }

  _stablePressed = readPressed();
  _lastRawPressed = _stablePressed;
  _pressedEvent = false;
  _lastChangeMs = millis();
}

void GpioButtonInput::tick(uint32_t nowMs) {
  if (_cfg.pin < 0) {
    return;
  }

  const bool rawPressed = readPressed();

  if (rawPressed != _lastRawPressed) {
    _lastRawPressed = rawPressed;
    _lastChangeMs = nowMs;
    return;
  }

  if ((nowMs - _lastChangeMs) < _cfg.debounceMs) {
    return;
  }

  if (rawPressed != _stablePressed) {
    _stablePressed = rawPressed;

    if (_stablePressed) {
      _pressedEvent = true;
    }
  }
}

bool GpioButtonInput::wasPressed() {
  const bool event = _pressedEvent;
  _pressedEvent = false;
  return event;
}

bool GpioButtonInput::readPressed() const {
  const int level = digitalRead(_cfg.pin);
  return _cfg.activeLow ? (level == LOW) : (level == HIGH);
}
