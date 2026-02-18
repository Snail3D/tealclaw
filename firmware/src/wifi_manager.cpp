#include "wifi_manager.h"
#include "key_store.h"
#include "config.h"

WiFiManager wifiManager;

void WiFiManager::begin() {
    // Build SSID from prefix + last 4 of MAC
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char suffix[5];
    snprintf(suffix, sizeof(suffix), "%02X%02X", mac[4], mac[5]);
    _apSSID = String(AP_SSID_PREFIX) + suffix;

    // Get AP password from NVS or use default
    String apPass = keyStore.get("wifi_pass");
    if (apPass.length() < 8) apPass = AP_DEFAULT_PASS;

    // Start AP
    WiFi.mode(WIFI_AP);
    WiFi.softAP(_apSSID.c_str(), apPass.c_str(), AP_CHANNEL, 0, AP_MAX_CLIENTS);

    Serial.printf("[WiFi] AP started: %s @ %s\n",
        _apSSID.c_str(), WiFi.softAPIP().toString().c_str());
}

void WiFiManager::beginSTA(const String& ssid, const String& pass) {
    if (ssid.length() == 0) return;

    WiFi.mode(WIFI_AP_STA);

    // Re-start AP (mode change drops it)
    String apPass = keyStore.get("wifi_pass");
    if (apPass.length() < 8) apPass = AP_DEFAULT_PASS;
    WiFi.softAP(_apSSID.c_str(), apPass.c_str(), AP_CHANNEL, 0, AP_MAX_CLIENTS);

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
