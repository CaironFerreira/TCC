#pragma once
#include <stddef.h>
#include <stdint.h>
#include "../domain/TelemetryFrame.h"

class ITelemetryDecoder {
public:
  virtual ~ITelemetryDecoder() = default;

  // Retorna true se conseguiu decodificar um frame válido
  virtual bool decode(const uint8_t* data, size_t len, TelemetryFrame& out) = 0;

  // Opcional: nome/identificador do layout (pra debug)
  virtual const char* name() const = 0;
};
