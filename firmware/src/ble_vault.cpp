#include "ble_vault.h"
#include "key_store.h"
#include "config.h"
#include <NimBLEDevice.h>
#include <ArduinoJson.h>

BleVault bleVault;

static NimBLEServer* pServer = nullptr;
static NimBLECharacteristic* pStatusChar = nullptr;
static NimBLECharacteristic* pConfigChar = nullptr;
static NimBLECharacteristic* pProximityChar = nullptr;

// ── Server callbacks ──
class VaultServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        Serial.println("[BLE] Client connected");
        bleVault.updateStatus(WiFi.status() == WL_CONNECTED);
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        Serial.printf("[BLE] Client disconnected (reason: %d)\n", reason);
        // Restart advertising so browser can reconnect
        NimBLEDevice::startAdvertising();
    }
};

void BleVault::begin() {
    NimBLEDevice::init(BLE_DEVICE_NAME);
    NimBLEDevice::setPower(ESP_PWR_LVL_P9); // Max range for proximity

    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new VaultServerCallbacks());

    // Create custom service
    NimBLEService* pService = pServer->createService(BLE_SERVICE_UUID);

    // Status characteristic (read + notify)
    pStatusChar = pService->createCharacteristic(
        BLE_CHAR_STATUS_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    pStatusChar->setValue("{\"connected\":true}");

    // Config characteristic (read only — non-sensitive config)
    pConfigChar = pService->createCharacteristic(
        BLE_CHAR_CONFIG_UUID,
        NIMBLE_PROPERTY::READ
    );

    // Proximity characteristic (read + notify — keepalive ping)
    pProximityChar = pService->createCharacteristic(
        BLE_CHAR_PROXIMITY_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    pProximityChar->setValue("ping");

    pService->start();

    // Advertising
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->start();

    Serial.println("[BLE] GATT server started, advertising");

    // Set initial config
    updateConfig();
}

void BleVault::updateStatus(bool wifiConnected) {
    if (!pStatusChar) return;

    JsonDocument doc;
    doc["connected"] = true;
    doc["wifi"] = wifiConnected;
    doc["uptime"] = millis() / 1000;
    doc["heap"] = ESP.getFreeHeap();

    String json;
    serializeJson(doc, json);
    pStatusChar->setValue(json);
    pStatusChar->notify();
}

void BleVault::updateConfig() {
    if (!pConfigChar) return;

    JsonDocument doc;
    doc["model"] = keyStore.get("ai_model");
    doc["voice"] = keyStore.get("tts_voice");
    doc["has_groq"] = keyStore.has("groq_key");
    doc["has_gemini"] = keyStore.has("gemini_key");

    String json;
    serializeJson(doc, json);
    pConfigChar->setValue(json);
}

bool BleVault::hasClient() {
    return pServer && pServer->getConnectedCount() > 0;
}

void BleVault::loop() {
    // Send proximity ping every 2 seconds
    if (pProximityChar && millis() - _lastProximityPing > 2000) {
        _lastProximityPing = millis();
        if (hasClient()) {
            char buf[16];
            snprintf(buf, sizeof(buf), "%lu", millis());
            pProximityChar->setValue(buf);
            pProximityChar->notify();
        }
    }
}
