/**
 * @file ArduRoombaBLE.cpp
 * @brief Implementation of BLE control for ESP32
 */

#include "ArduRoombaBLE.h"

#if defined(ESP32)

// BLE Server callbacks
class ArduRoombaBLE::ServerCallbacks: public BLEServerCallbacks {
  ArduRoombaBLE* _parent;
public:
  ServerCallbacks(ArduRoombaBLE* parent) : _parent(parent) {}

  void onConnect(BLEServer* server) {
    _parent->_deviceConnected = true;
    _parent->_connectionCount++;
    Serial.println("BLE Client connected");
  }

  void onDisconnect(BLEServer* server) {
    _parent->_deviceConnected = false;
    Serial.println("BLE Client disconnected");

    // Restart advertising
    delay(500);
    server->startAdvertising();
    Serial.println("BLE Advertising restarted");
  }
};

// Command characteristic callbacks
class ArduRoombaBLE::CommandCallbacks: public BLECharacteristicCallbacks {
  ArduRoombaBLE* _parent;
public:
  CommandCallbacks(ArduRoombaBLE* parent) : _parent(parent) {}

  void onWrite(BLECharacteristic* characteristic) {
    String value = characteristic->getValue().c_str();
    if (value.length() > 0) {
      Serial.print("Received BLE command: ");
      Serial.println(value);
      _parent->processCommand(value);
    }
  }
};

ArduRoombaBLE::ArduRoombaBLE(ArduRoomba& roomba, const char* deviceName)
  : _roomba(roomba), _deviceName(deviceName), _remoteEnabled(true),
    _deviceConnected(false), _oldDeviceConnected(false), _connectionCount(0),
    _commandCallback(nullptr), _server(nullptr), _service(nullptr),
    _commandChar(nullptr), _statusChar(nullptr), _lastStatusUpdate(0) {
}

ArduRoombaBLE::~ArduRoombaBLE() {
  end();
}

bool ArduRoombaBLE::begin() {
  Serial.println("Initializing BLE...");

  // Initialize BLE
  BLEDevice::init(_deviceName.c_str());

  // Create BLE Server
  _server = BLEDevice::createServer();
  _server->setCallbacks(new ServerCallbacks(this));

  // Create BLE Service
  _service = _server->createService(SERVICE_UUID);

  // Create Command Characteristic (Write)
  _commandChar = _service->createCharacteristic(
    COMMAND_CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  _commandChar->setCallbacks(new CommandCallbacks(this));

  // Create Status Characteristic (Read + Notify)
  _statusChar = _service->createCharacteristic(
    STATUS_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  _statusChar->addDescriptor(new BLE2902());

  // Set initial status
  String status = generateStatus();
  _statusChar->setValue(status.c_str());

  // Start the service
  _service->start();

  // Start advertising
  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->setScanResponse(true);
  advertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  advertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE Service started");
  Serial.print("Device name: ");
  Serial.println(_deviceName);
  Serial.println("Waiting for connections...");

  return true;
}

void ArduRoombaBLE::end() {
  if (_server) {
    BLEDevice::deinit(true);
    _server = nullptr;
    _service = nullptr;
    _commandChar = nullptr;
    _statusChar = nullptr;
  }
}

void ArduRoombaBLE::updateStatus() {
  // Handle connection state changes
  if (_deviceConnected && !_oldDeviceConnected) {
    _oldDeviceConnected = _deviceConnected;
  }

  if (!_deviceConnected && _oldDeviceConnected) {
    _oldDeviceConnected = _deviceConnected;
  }

  // Periodically update status characteristic
  if (_deviceConnected && (millis() - _lastStatusUpdate > STATUS_UPDATE_INTERVAL)) {
    String status = generateStatus();
    _statusChar->setValue(status.c_str());
    _statusChar->notify();
    _lastStatusUpdate = millis();
  }
}

void ArduRoombaBLE::setCommandCallback(void (*callback)(const String&)) {
  _commandCallback = callback;
}

void ArduRoombaBLE::processCommand(const String& command) {
  if (!_remoteEnabled) {
    Serial.println("Remote control disabled");
    return;
  }

  // Call user callback if set
  if (_commandCallback) {
    _commandCallback(command);
  }

  // Parse command format: "ACTION:SPEED:DURATION"
  // Examples: "forward:200:0", "left:150:1000", "stop:0:0"
  int firstColon = command.indexOf(':');
  int secondColon = command.indexOf(':', firstColon + 1);

  String action = command.substring(0, firstColon);
  int speed = 200;
  int duration = 0;

  if (firstColon > 0) {
    if (secondColon > firstColon) {
      speed = command.substring(firstColon + 1, secondColon).toInt();
      duration = command.substring(secondColon + 1).toInt();
    } else {
      speed = command.substring(firstColon + 1).toInt();
    }
  }

  // Process commands
  if (action == "forward") {
    _roomba.moveForward(speed);
  }
  else if (action == "backward") {
    _roomba.moveBackward(speed);
  }
  else if (action == "left") {
    _roomba.turnLeft(speed);
  }
  else if (action == "right") {
    _roomba.turnRight(speed);
  }
  else if (action == "stop") {
    _roomba.stop();
  }
  else if (action == "clean") {
    _roomba.startCleaning();
  }
  else if (action == "spot") {
    _roomba.spotClean();
  }
  else if (action == "dock") {
    _roomba.dock();
  }
  else if (action == "beep") {
    _roomba.beep();
  }

  // Handle timed commands
  if (duration > 0 && action != "stop") {
    delay(duration);
    _roomba.stop();
  }
}

String ArduRoombaBLE::generateStatus() {
  uint16_t voltage = _roomba.getBatteryVoltage();
  bool connected = _roomba.isConnected();
  bool wall = _roomba.isWallDetected();
  bool bumper = _roomba.isBumperPressed();

  // Format: "voltage:connected:wall:bumper:remote"
  String status = String(voltage) + ":";
  status += (connected ? "1" : "0") + String(":");
  status += (wall ? "1" : "0") + String(":");
  status += (bumper ? "1" : "0") + String(":");
  status += (_remoteEnabled ? "1" : "0");

  return status;
}

#endif // ESP32
