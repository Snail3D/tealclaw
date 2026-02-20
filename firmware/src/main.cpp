#include <Arduino.h>
#include <LittleFS.h>
#include "config.h"
#include "key_store.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "ble_vault.h"

// ── LED states ──
enum LedMode { LED_OFF, LED_BLINK, LED_SOLID };
static LedMode ledMode = LED_OFF;
static unsigned long lastBlink = 0;

static void setLed(LedMode mode) {
    ledMode = mode;
    if (mode == LED_SOLID) digitalWrite(PIN_LED, HIGH);
    if (mode == LED_OFF) digitalWrite(PIN_LED, LOW);
}

static void ledTick() {
    if (ledMode == LED_BLINK && millis() - lastBlink > 500) {
        lastBlink = millis();
        digitalWrite(PIN_LED, !digitalRead(PIN_LED));
    }
}

// ── Factory reset check ──
static unsigned long resetBtnDown = 0;
static bool resetTriggered = false;

static void checkFactoryReset() {
    if (digitalRead(PIN_RESET_BTN) == LOW) {
        if (resetBtnDown == 0) resetBtnDown = millis();
        if (!resetTriggered && millis() - resetBtnDown >= FACTORY_RESET_MS) {
            resetTriggered = true;
            Serial.println("[RESET] Factory reset triggered!");
            // Rapid blink to indicate reset
            for (int i = 0; i < 10; i++) {
                digitalWrite(PIN_LED, !digitalRead(PIN_LED));
                delay(100);
            }
            keyStore.clear();
            ESP.restart();
        }
    } else {
        resetBtnDown = 0;
        resetTriggered = false;
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=== TealClaw Hardware Vault ===");

    // GPIO
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_RESET_BTN, INPUT_PULLUP);
    setLed(LED_BLINK); // Blinking = starting up

    // Init LittleFS
    if (!LittleFS.begin(true)) {
        Serial.println("[FS] LittleFS mount failed!");
    } else {
        Serial.println("[FS] LittleFS mounted");
    }

    // Init key store
    keyStore.begin();

    bool setupMode = !keyStore.hasKeys();
    Serial.printf("[Boot] Mode: %s\n", setupMode ? "SETUP" : "GATEWAY");

    // Start WiFi AP
    wifiManager.begin();

    // If keys configured, also try STA for internet access
    if (!setupMode) {
        String staSsid = keyStore.get("wifi_ssid");
        String staPass = keyStore.get("wifi_pass_sta");

        // Backward compatibility: older builds accidentally stored STA pass under wifi_pass.
        if (staSsid.length() > 0 && staPass.length() == 0) {
            String legacyPass = keyStore.get("wifi_pass");
            if (legacyPass.length() > 0) {
                Serial.println("[Boot] Using legacy wifi_pass key for STA password fallback");
                staPass = legacyPass;
            }
        }

        if (staSsid.length() > 0) {
            wifiManager.beginSTA(staSsid, staPass);
        }
    }

    // Start web server
    tcWebServer.begin(setupMode);

    // Start BLE only in gateway mode
    if (!setupMode) {
        bleVault.begin();
        setLed(LED_SOLID); // Solid = gateway active
    } else {
        setLed(LED_BLINK); // Blinking = setup mode
    }

    Serial.println("[Boot] Ready!");
    Serial.printf("[Boot] AP IP: %s\n", wifiManager.getAPIP().toString().c_str());
    Serial.printf("[Boot] AP SSID: %s\n", wifiManager.getAPSSID().c_str());
}

void loop() {
    ledTick();
    checkFactoryReset();
    bleVault.loop();

    // Periodically update BLE status
    static unsigned long lastStatusUpdate = 0;
    if (millis() - lastStatusUpdate > 10000) {
        lastStatusUpdate = millis();
        bleVault.updateStatus(wifiManager.isConnectedToInternet());
    }

    delay(10); // Yield to RTOS
}
