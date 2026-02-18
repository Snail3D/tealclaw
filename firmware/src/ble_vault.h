#pragma once
#include <Arduino.h>

class BleVault {
public:
    void begin();
    void updateStatus(bool wifiConnected);
    void updateConfig();
    bool hasClient();
    void loop(); // Proximity notify tick

private:
    bool _clientConnected = false;
    unsigned long _lastProximityPing = 0;
};

extern BleVault bleVault;
