#pragma once

#include <Arduino.h>

class IWifiConfigPortal {
public:
  virtual ~IWifiConfigPortal() = default;

  virtual bool begin() = 0;
  virtual void tick() = 0;

  virtual bool isConnected() const = 0;
  virtual bool isPortalActive() const = 0;

  virtual String localIp() const = 0;
  virtual String connectedSsid() const = 0;
};
