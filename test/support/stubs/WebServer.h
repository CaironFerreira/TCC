#pragma once

#include <functional>
#include <map>
#include <string>

#include "Arduino.h"

static const int HTTP_GET = 0;
static const int HTTP_POST = 1;

namespace webserver_stub {

struct State {
  uint16_t lastPort = 0;
  uint32_t beginCalls = 0;
  uint32_t stopCalls = 0;
  uint32_t handleClientCalls = 0;
  std::map<std::string, String> args;
};

inline State& state() {
  static State value;
  return value;
}

inline void reset() {
  state() = State{};
}

inline void setArg(const char* key, const char* value) {
  state().args[key != nullptr ? key : ""] = value != nullptr ? value : "";
}

}  // namespace webserver_stub

class WebServer {
public:
  explicit WebServer(uint16_t port) {
    webserver_stub::state().lastPort = port;
  }

  template <typename Handler>
  void on(const char* uri, int, Handler handler) {
    _handlers[uri != nullptr ? uri : ""] = handler;
  }

  template <typename Handler>
  void onNotFound(Handler handler) {
    _notFoundHandler = handler;
  }

  void begin() {
    ++webserver_stub::state().beginCalls;
  }

  void stop() {
    ++webserver_stub::state().stopCalls;
  }

  void handleClient() {
    ++webserver_stub::state().handleClientCalls;
  }

  void sendHeader(const char*, const char*, bool = false) {}

  void send(int, const char*, const String&) {}

  void send(int, const char*, const char*) {}

  String arg(const char* name) const {
    const auto it = webserver_stub::state().args.find(name != nullptr ? name : "");
    if (it == webserver_stub::state().args.end()) {
      return String("");
    }
    return it->second;
  }

private:
  std::map<std::string, std::function<void()>> _handlers;
  std::function<void()> _notFoundHandler;
};
