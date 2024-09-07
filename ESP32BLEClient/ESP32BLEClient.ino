#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>
#include <BLEAddress.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

BLEUUID serviceUUID("your-service-uuid");   // UUID of the service you want to connect to
BLEUUID characteristicUUID("your-characteristic-uuid");  // UUID of the characteristic you want to read

BLEClient* pClient;
BLERemoteCharacteristic* pRemoteCharacteristic;

bool deviceConnected = false;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("Found device: ");
    Serial.println(advertisedDevice.toString().c_str());

    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      Serial.println("Device with correct service UUID found.");
      advertisedDevice.getScan()->stop();  // Stop scanning
      pClient->connect(&advertisedDevice);  // Connect to the advertised device
      deviceConnected = true;
    }
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("");
  pClient = BLEDevice::createClient();

  // Set up BLE scan
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30); // Scan for 30 seconds
}

void loop() {
  if (deviceConnected) {
    if (pRemoteCharacteristic == nullptr) {
      // Obtain a reference to the remote characteristic by UUID
      BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
      if (pRemoteService == nullptr) {
        Serial.println("Failed to find the service UUID.");
        pClient->disconnect();
        return;
      }
      pRemoteCharacteristic = pRemoteService->getCharacteristic(characteristicUUID);
      if (pRemoteCharacteristic == nullptr) {
        Serial.println("Failed to find the characteristic UUID.");
        pClient->disconnect();
        return;
      }
    }
    
    // Read the value from the remote characteristic
    if (pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("Received value: ");
      Serial.println(value.c_str());
    }
    
    delay(2000); // Poll every 2 seconds
  }
}
