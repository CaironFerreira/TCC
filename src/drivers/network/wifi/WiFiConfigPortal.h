#pragma once

#include <Arduino.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <WebServer.h>
#include <WiFi.h>
#include "ports/IWifiConfigPortal.h"

class WiFiConfigPortal : public IWifiConfigPortal {
public:
  struct Config {
    const char* apSsid = "SimHub";
    const char* apPassword = nullptr;
    uint32_t connectTimeoutMs = 8000;
  };

  enum class ConnectResult : uint8_t {
    Success,
    EmptySsid,
    InvalidPassword,
    Timeout,
    AuthFailed,
    Failed
  };

  enum class PortalState : uint8_t {
    Idle,
    Connecting,
    Success,
    Error
  };

public:
  WiFiConfigPortal();
  explicit WiFiConfigPortal(const Config& cfg);

  WiFiConfigPortal(const WiFiConfigPortal&) = delete;
  WiFiConfigPortal& operator=(const WiFiConfigPortal&) = delete;

  bool begin() override;
  void tick() override;

  bool isConnected() const override;
  bool isPortalActive() const override;

  String localIp() const override;
  String connectedSsid() const override;
  String lastStatusMessage() const;
  String connectionState() const;

  void clearCredentials();

private:
  void loadCredentials();
  void saveCredentials(const String& ssid, const String& password);

  ConnectResult tryConnect();
  void beginConnectionAttempt(const String& ssid, const String& password);
  void processConnectionAttempt();
  void retrySavedCredentialsIfNeeded();

  void startPortal();
  void stopPortal();
  void setupRoutes();

  void handleRoot();
  void handleSave();
  void handleStatus();
  void handleNotFound();

  void configureWifiBase();

  const char* connectResultMessage(ConnectResult result) const;
  bool isValidSsid(const String& ssid) const;
  bool isValidPassword(const String& password) const;

private:
  enum : uint16_t {
    DNS_PORT = 53
  };
  static const uint32_t PORTAL_CLOSE_DELAY_MS = 2000;
  static const uint32_t SAVED_RETRY_INTERVAL_MS = 10000;

  Config _cfg;
  DNSServer _dnsServer;
  WebServer _server;
  Preferences _prefs;

  String _ssid;
  String _password;
  String _lastStatusMessage;

  String _pendingSsid;
  String _pendingPassword;

  bool _portalActive = false;
  bool _connectRequested = false;
  bool _portalStopPending = false;

  PortalState _state = PortalState::Idle;
  ConnectResult _lastResult = ConnectResult::Failed;

  uint32_t _connectStartMs = 0;
  uint32_t _successAtMs = 0;
  uint32_t _lastAttemptEndMs = 0;
};
