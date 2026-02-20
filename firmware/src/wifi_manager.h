#pragma once
#include <Arduino.h>
#include <WiFi.h>

class WiFiManager {
public:
    void begin();
    void beginSTA(const String& ssid, const String& pass);
    IPAddress getAPIP();
    bool isConnectedToInternet();
    String getAPSSID();
    String getAPPassword();

private:
    String _apSSID;
    String _apPass;

    String _deriveDefaultAPPassword();
};

extern WiFiManager wifiManager;
