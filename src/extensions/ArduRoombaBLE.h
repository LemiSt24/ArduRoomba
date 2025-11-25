/**
 * @file ArduRoombaBLE.h
 * @brief Bluetooth Low Energy extension for ESP32
 *
 * Provides BLE GATT server for mobile app control of Roomba.
 * Uses standard BLE services and characteristics for easy integration
 * with mobile apps and BLE explorers.
 *
 * BLE Service UUID: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
 * Command Characteristic: beb5483e-36e1-4688-b7f5-ea07361b26a8 (Write)
 * Status Characteristic: beb5483f-36e1-4688-b7f5-ea07361b26a8 (Read/Notify)
 */

#ifndef ARDUROOMBA_BLE_H
#define ARDUROOMBA_BLE_H

#include "../ArduRoomba.h"

// Only compile for ESP32
#if defined(ESP32)

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// BLE UUIDs
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define COMMAND_CHAR_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define STATUS_CHAR_UUID    "beb5483f-36e1-4688-b7f5-ea07361b26a8"

/**
 * BLE extension for ESP32 Roomba control
 * Provides GATT server for mobile app connectivity
 */
class ArduRoombaBLE {
public:
  ArduRoombaBLE(ArduRoomba& roomba, const char* deviceName = "ArduRoomba");
  ~ArduRoombaBLE();

  // BLE setup
  bool begin();
  void end();

  // Status
  bool isConnected() const { return _deviceConnected; }
  int getConnectionCount() const { return _connectionCount; }

  // Update status (call in loop to send notifications)
  void updateStatus();

  // Command callback
  void setCommandCallback(void (*callback)(const String&));

  // Enable/disable remote control
  void enableRemoteControl(bool enable) { _remoteEnabled = enable; }
  bool isRemoteEnabled() const { return _remoteEnabled; }

private:
  ArduRoomba& _roomba;
  String _deviceName;
  bool _remoteEnabled;
  bool _deviceConnected;
  bool _oldDeviceConnected;
  int _connectionCount;
  void (*_commandCallback)(const String&);

  BLEServer* _server;
  BLEService* _service;
  BLECharacteristic* _commandChar;
  BLECharacteristic* _statusChar;

  unsigned long _lastStatusUpdate;
  static const unsigned long STATUS_UPDATE_INTERVAL = 2000; // 2 seconds

  void processCommand(const String& command);
  String generateStatus();

  // BLE callback classes
  class ServerCallbacks;
  class CommandCallbacks;

  friend class ServerCallbacks;
  friend class CommandCallbacks;
};

#endif // ESP32
#endif // ARDUROOMBA_BLE_H
