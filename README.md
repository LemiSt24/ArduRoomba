# ArduRoomba - Clean & Minimal

A minimal, focused Arduino library for iRobot Open Interface communication.

[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/pkyanam/ArduRoomba)

## Design Philosophy

- **Minimal Core**: Essential functionality only
- **Smart Modularity**: Clean interfaces for future extensions
- **No Bloat**: Simple, readable code without over-engineering
- **Extensible**: Easy to add WiFi, Bluetooth, RTOS when needed

## Architecture

```
ArduRoomba/
├── src/
│   ├── ArduRoomba.h/.cpp          # Main library interface
│   ├── RoombaOI.h/.cpp            # Open Interface communication
│   └── extensions/                # WiFi & BLE control
│       ├── ArduRoombaWiFi.*       # Base WiFi interface
│       ├── ArduRoombaWiFiS3.*     # Uno R4 WiFi implementation
│       ├── ArduRoombaESP32WiFi.*  # ESP32 WiFi implementation
│       └── ArduRoombaBLE.*        # ESP32 BLE implementation
└── examples/
    ├── BasicMovement/
    ├── SensorReading/
    ├── SimpleControl/
    ├── WiFiControl_UnoR4/         # WiFi control (Uno R4 WiFi)
    ├── WiFiControl_ESP32/         # WiFi control (ESP32)
    └── BLEControl_ESP32/          # Bluetooth control (ESP32)
```

## Core Features

- iRobot Open Interface protocol implementation
- Basic movement commands (drive, turn, stop)
- Sensor reading (individual and streaming)
- Simple LED and sound control
- Clean error handling (bool returns, no complex enums)

## Wireless Extensions

### WiFi Control (Arduino Uno R4 WiFi & ESP32)
- **Access Point Mode**: Roomba creates its own WiFi network
- **Client Mode**: Connect to existing WiFi network
- **Web Interface**: Browser-based control with responsive UI
- **HTTP API**: RESTful endpoints for custom integrations
- **Real-time Status**: Battery, sensors, connection state

### Bluetooth Low Energy (ESP32 only)
- **GATT Server**: Standard BLE profile for mobile apps
- **Low Power**: Energy-efficient wireless control
- **Status Notifications**: Real-time sensor updates
- **Mobile Ready**: Compatible with nRF Connect, LightBlue, custom apps
- **Simple Protocol**: Easy command format for integration

## Future Extensions

- RTOS integration
- Advanced sensor processing
- Custom autonomous behaviors

## Usage

### Basic Control

```cpp
#include "ArduRoomba.h"

ArduRoomba roomba(2, 3, 4); // RX, TX, BRC pins

void setup() {
  Serial.begin(19200);
  if (roomba.begin()) {
    Serial.println("Roomba connected!");
    roomba.moveForward();
    delay(2000);
    roomba.stop();
  }
}
```

### WiFi Control (Arduino Uno R4 WiFi)

```cpp
#include "ArduRoomba.h"
#include "extensions/ArduRoombaWiFiS3.h"

ArduRoomba roomba(2, 3, 4);
ArduRoombaWiFiS3 wifi(roomba);

void setup() {
  Serial.begin(19200);
  roomba.begin();

  wifi.beginAP("ArduRoomba", "roomba123");
  wifi.startWebServer();

  Serial.print("Control at: http://");
  Serial.println(wifi.getIPAddress());
}

void loop() {
  wifi.handleClient();
}
```

### WiFi Control (ESP32)

```cpp
#include "ArduRoomba.h"
#include "extensions/ArduRoombaESP32WiFi.h"

ArduRoomba roomba(16, 17, 5);
ArduRoombaESP32WiFi wifi(roomba);

void setup() {
  Serial.begin(115200);
  roomba.begin();

  wifi.beginAP("ArduRoomba-ESP32", "roomba123");
  wifi.startWebServer();

  Serial.print("Control at: http://");
  Serial.println(wifi.getIPAddress());
}

void loop() {
  wifi.handleClient();
}
```

### BLE Control (ESP32)

```cpp
#include "ArduRoomba.h"
#include "extensions/ArduRoombaBLE.h"

ArduRoomba roomba(16, 17, 5);
ArduRoombaBLE ble(roomba, "ArduRoomba");

void setup() {
  Serial.begin(115200);
  roomba.begin();
  ble.begin();

  Serial.println("Connect via BLE app (nRF Connect, LightBlue)");
}

void loop() {
  ble.updateStatus();
}
```

## Supported Hardware

- Arduino Uno R3/R4
- ESP32/ESP8266
- iRobot Create 2, Roomba 500/600/700 series

## License

MIT License - See [LICENSE](LICENSE) file for details.

This project is free for commercial and non-commercial use.
