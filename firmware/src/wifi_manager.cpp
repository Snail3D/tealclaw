#include "wifi_manager.h"
#include "key_store.h"
#include "config.h"

WiFiManager wifiManager;

String WiFiManager::_deriveDefaultAPPassword() {
    uint64_t chipId = ESP.getEfuseMac();
    char pass[16];
    // Device-unique, deterministic fallback (10 chars)
    snprintf(pass, sizeof(pass), "tc%08lx", (unsigned long)(chipId & 0xFFFFFFFF));
    return String(pass);
}

void WiFiManager::begin() {
    // Build SSID from prefix + last 4 of MAC
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char suffix[5];
    snprintf(suffix, sizeof(suffix), "%02X%02X", mac[4], mac[5]);
    _apSSID = String(AP_SSID_PREFIX) + suffix;

    // Get AP password from NVS or use hardened per-device default
    _apPass = keyStore.get("wifi_pass");
    bool usingDefaultPass = false;
    if (_apPass.length() < 8) {
        _apPass = _deriveDefaultAPPassword();
        usingDefaultPass = true;
    }

    // Start AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(_apSSID.c_str(), _apPass.c_str(), AP_CHANNEL, 0, AP_MAX_CLIENTS);

    Serial.printf("[WiFi] AP started: %s @ %s\n",
        _apSSID.c_str(), WiFi.softAPIP().toString().c_str());
    if (usingDefaultPass) {
        Serial.printf("[WiFi] AP default password (device-unique): %s\n", _apPass.c_str());
    }
}

void WiFiManager::beginSTA(const String& ssid, const String& pass) {
    if (ssid.length() == 0) return;

    WiFi.mode(WIFI_AP_STA);

    // Re-start AP (mode change drops it)
    if (_apPass.length() < 8) {
        _apPass = _deriveDefaultAPPassword();
    }
    WiFi.softAP(_apSSID.c_str(), _apPass.c_str(), AP_CHANNEL, 0, AP_MAX_CLIENTS);

    WiFi.begin(ssid.c_str(), pass.c_str());
    Serial.printf("[WiFi] Connecting to STA: %s\n", ssid.c_str());

    // Non-blocking — isConnectedToInternet() checks later
}

IPAddress WiFiManager::getAPIP() {
    return WiFi.softAPIP();
}

bool WiFiManager::isConnectedToInternet() {
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getAPSSID() {
    return _apSSID;
}

String WiFiManager::getAPPassword() {
    return _apPass;
}
