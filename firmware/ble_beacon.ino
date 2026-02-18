/*
 * TealClaw BLE Proximity Beacon
 *
 * Advertises as "TC-Beacon" with headphones appearance so it looks
 * like a normal audio device.  Accepts one GATT connection at a time
 * and re-advertises on disconnect.  No data is exchanged — the browser
 * only needs to see the device is connectable and in range.
 *
 * Board: ESP32 (any variant with BLE)
 * Framework: Arduino  (PlatformIO or Arduino IDE)
 */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <esp_bt.h>

#define DEVICE_NAME "TC-Beacon"
#define SERVICE_UUID "0000180d-0000-1000-8000-00805f9b34fb" // Heart Rate

static BLEServer *pServer = nullptr;
static bool connected = false;

class ServerCB : public BLEServerCallbacks {
  void onConnect(BLEServer *s)    override { connected = true;  }
  void onDisconnect(BLEServer *s) override { connected = false; }
};

void setup() {
  BLEDevice::init(DEVICE_NAME);

  // Minimum TX power for short range (~5-10 m)
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_N12);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV,     ESP_PWR_LVL_N12);
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_SCAN,    ESP_PWR_LVL_N12);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCB());

  // Create a dummy Heart Rate service so the device is connectable
  BLEService *svc = pServer->createService(SERVICE_UUID);
  svc->start();

  // Advertising
  BLEAdvertising *adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->setAppearance(0x0341);          // Headphones
  adv->setScanResponse(true);
  adv->setMinPreferred(0x06);
  BLEDevice::startAdvertising();
}

void loop() {
  // Re-advertise after a client disconnects
  if (!connected) {
    delay(500);
    BLEDevice::startAdvertising();
  }
  delay(1000);
}
