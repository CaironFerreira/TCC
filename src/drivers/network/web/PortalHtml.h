#pragma once

#include <Arduino.h>

class PortalHtml {
public:
  static String page(const char* deviceName);
  static String errorPage(const char* deviceName, const String& ssid, const char* message);
  static String successPage(const String& ip);

private:
  static String basePage(const String& title, const String& content);
  static String htmlEscape(const String& value);
};