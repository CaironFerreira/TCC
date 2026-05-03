#pragma once

#include <stddef.h>

class IWifiConfigPortal {
public:
  virtual ~IWifiConfigPortal() = default;

  // Starts the Wi-Fi portal/connection state machine without waiting for a
  // final connection result.
  virtual void begin() = 0;
  virtual void tick() = 0;

  virtual bool isConnected() const = 0;
  virtual bool isPortalActive() const = 0;

  virtual void copyLocalIp(char* buffer, size_t bufferSize) const = 0;
  virtual void copyConnectedSsid(char* buffer, size_t bufferSize) const = 0;
};
