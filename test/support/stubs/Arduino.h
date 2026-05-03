#pragma once

#include <algorithm>
#include <cctype>
#include <stdint.h>
#include <string>
#include <stdint.h>

#define INPUT 0x01
#define INPUT_PULLUP 0x02
#define INPUT_PULLDOWN 0x03

#define LOW 0x0
#define HIGH 0x1
#define F(x) x

class String {
public:
  String() = default;
  String(const char* value) : _value(value != nullptr ? value : "") {}
  String(const std::string& value) : _value(value) {}
  String(char value) : _value(1, value) {}
  String(int value) : _value(std::to_string(value)) {}
  String(unsigned long value) : _value(std::to_string(value)) {}

  String& operator=(const char* value) {
    _value = (value != nullptr) ? value : "";
    return *this;
  }

  String& operator+=(const String& other) {
    _value += other._value;
    return *this;
  }

  String& operator+=(const char* other) {
    if (other != nullptr) {
      _value += other;
    }
    return *this;
  }

  String& operator+=(char other) {
    _value += other;
    return *this;
  }

  bool isEmpty() const {
    return _value.empty();
  }

  size_t length() const {
    return _value.length();
  }

  void reserve(size_t size) {
    _value.reserve(size);
  }

  void trim() {
    const auto notSpace = [](unsigned char ch) { return !std::isspace(ch); };
    const auto begin = std::find_if(_value.begin(), _value.end(), notSpace);
    const auto end = std::find_if(_value.rbegin(), _value.rend(), notSpace).base();

    if (begin >= end) {
      _value.clear();
      return;
    }

    _value.assign(begin, end);
  }

  const char* c_str() const {
    return _value.c_str();
  }

  char operator[](size_t index) const {
    return _value[index];
  }

  char& operator[](size_t index) {
    return _value[index];
  }

  bool operator==(const String& other) const {
    return _value == other._value;
  }

  bool operator==(const char* other) const {
    return _value == (other != nullptr ? other : "");
  }

  bool operator!=(const String& other) const {
    return !(*this == other);
  }

  bool operator!=(const char* other) const {
    return !(*this == other);
  }

private:
  std::string _value;
};

inline String operator+(String lhs, const String& rhs) {
  lhs += rhs;
  return lhs;
}

namespace arduino_stub {

struct PinState {
  int mode = -1;
  int level = HIGH;
  uint32_t pinModeCalls = 0;
  uint32_t digitalReadCalls = 0;
};

inline PinState* pins() {
  static PinState state[64];
  return state;
}

inline uint32_t& currentMillis() {
  static uint32_t value = 0;
  return value;
}

inline void reset() {
  for (int i = 0; i < 64; ++i) {
    pins()[i] = PinState{};
  }
  currentMillis() = 0;
}

inline void setMillis(uint32_t value) {
  currentMillis() = value;
}

inline void advanceMillis(uint32_t delta) {
  currentMillis() += delta;
}

inline void setPinLevel(int pin, int level) {
  if (pin >= 0 && pin < 64) {
    pins()[pin].level = level;
  }
}

inline int pinModeValue(int pin) {
  return (pin >= 0 && pin < 64) ? pins()[pin].mode : -1;
}

inline uint32_t pinModeCalls(int pin) {
  return (pin >= 0 && pin < 64) ? pins()[pin].pinModeCalls : 0;
}

inline uint32_t digitalReadCalls(int pin) {
  return (pin >= 0 && pin < 64) ? pins()[pin].digitalReadCalls : 0;
}

}  // namespace arduino_stub

inline uint32_t millis() {
  return arduino_stub::currentMillis();
}

inline void delay(uint32_t ms) {
  arduino_stub::advanceMillis(ms);
}

template <typename T>
inline T constrain(T value, T low, T high) {
  if (value < low) {
    return low;
  }
  if (value > high) {
    return high;
  }
  return value;
}

inline void pinMode(int pin, int mode) {
  if (pin >= 0 && pin < 64) {
    arduino_stub::pins()[pin].mode = mode;
    ++arduino_stub::pins()[pin].pinModeCalls;
  }
}

inline int digitalRead(int pin) {
  if (pin >= 0 && pin < 64) {
    ++arduino_stub::pins()[pin].digitalReadCalls;
    return arduino_stub::pins()[pin].level;
  }

  return LOW;
}
