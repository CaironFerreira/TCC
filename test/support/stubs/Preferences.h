#pragma once

#include <map>
#include <string>

#include "Arduino.h"

namespace preferences_stub {

using NamespaceStore = std::map<std::string, std::string>;

inline std::map<std::string, NamespaceStore>& storage() {
  static std::map<std::string, NamespaceStore> value;
  return value;
}

inline void reset() {
  storage().clear();
}

inline void setString(const char* ns, const char* key, const char* value) {
  storage()[ns != nullptr ? ns : ""][key != nullptr ? key : ""] =
      value != nullptr ? value : "";
}

inline String getString(const char* ns, const char* key) {
  const std::string nsKey = ns != nullptr ? ns : "";
  const std::string itemKey = key != nullptr ? key : "";
  const auto nsIt = storage().find(nsKey);
  if (nsIt == storage().end()) {
    return String("");
  }

  const auto itemIt = nsIt->second.find(itemKey);
  if (itemIt == nsIt->second.end()) {
    return String("");
  }

  return String(itemIt->second);
}

}  // namespace preferences_stub

class Preferences {
public:
  bool begin(const char* name, bool readOnly) {
    _namespaceName = name != nullptr ? name : "";
    _readOnly = readOnly;
    return true;
  }

  String getString(const char* key, const char* defaultValue = "") const {
    const String value = preferences_stub::getString(_namespaceName.c_str(), key);
    if (value.isEmpty()) {
      return String(defaultValue);
    }
    return value;
  }

  size_t putString(const char* key, const String& value) {
    if (_readOnly) {
      return 0;
    }

    preferences_stub::setString(_namespaceName.c_str(), key, value.c_str());
    return value.length();
  }

  bool clear() {
    if (_readOnly) {
      return false;
    }

    preferences_stub::storage()[_namespaceName].clear();
    return true;
  }

  void end() {}

private:
  std::string _namespaceName;
  bool _readOnly = false;
};
