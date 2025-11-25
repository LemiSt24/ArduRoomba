# ArduRoomba - Breathe New Life Into Your Roomba

**The definitive Arduino library for controlling legacy iRobot Roomba robots with WiFi and Bluetooth.**

[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/pkyanam/ArduRoomba)

Turn your old Roomba into a smart, wirelessly-controlled robot. This library supports iRobot Create 2 and Roomba 500/600/700 series robots that would otherwise end up as e-waste. With WiFi web control and Bluetooth LE support, your decade-old Roomba can become a modern IoT device.

## Why ArduRoomba?

Millions of perfectly functional Roomba robots sit in closets or landfills because their apps were discontinued or batteries died. The motors, sensors, and chassis are still good. ArduRoomba lets you:

- **Revive old hardware** - Connect an Arduino or ESP32 and control via WiFi or Bluetooth
- **Build custom robots** - Use the Roomba chassis as a base for your own projects
- **Learn robotics** - Affordable platform with real sensors and actuators
- **Reduce e-waste** - Give your old robot a second life instead of throwing it away

## Features

### Core Functionality
- Complete iRobot Open Interface (OI) protocol implementation
- Movement control (drive, turn, spin, stop)
- Sensor reading (bumpers, cliff, wall, battery, buttons)
- LED control and speaker tones
- Safe and Full mode operation
- Debug logging support

### WiFi Control (v3.1.0+)
Control your Roomba from any web browser on your network:

| Feature | Arduino Uno R4 WiFi | ESP32 |
|---------|---------------------|-------|
| Access Point Mode | ✓ | ✓ |
| Client/Station Mode | ✓ | ✓ |
| Web Interface | ✓ | ✓ |
| HTTP REST API | ✓ | ✓ |
| Real-time Status | ✓ | ✓ |

**Web Interface Features:**
- Responsive mobile-friendly design
- Directional controls (forward, back, left, right, spin)
- Adjustable speed and duration
- Live battery voltage display
- Sensor status indicators

### Bluetooth Low Energy (v3.1.0+, ESP32 only)
Control your Roomba from mobile apps:

- **GATT Server** with standard UUIDs for maximum compatibility
- **Command Characteristic** - Send movement commands
- **Status Characteristic** - Read sensor data with notifications
- **Works with** nRF Connect, LightBlue, or build your own app
- **Low power** - BLE is energy-efficient for battery-powered projects

**BLE Protocol:**
- Command format: `action:speed:duration` (e.g., `forward:200:1000`)
- Status format: `voltage:connected:wall:bumper:remote`
- Service UUID: `4fafc201-1fb5-459e-8fcc-c5c9c331914b`

## Architecture

```
ArduRoomba/
├── src/
│   ├── ArduRoomba.h/.cpp          # High-level interface
│   ├── RoombaOI.h/.cpp            # Low-level OI protocol
│   └── extensions/                # Wireless modules
│       ├── ArduRoombaWiFi.*       # WiFi base class
│       ├── ArduRoombaWiFiS3.*     # Arduino Uno R4 WiFi
│       ├── ArduRoombaESP32WiFi.*  # ESP32 WiFi
│       └── ArduRoombaBLE.*        # ESP32 Bluetooth LE
└── examples/
    ├── BasicMovement/             # Getting started
    ├── SensorReading/             # Reading sensors
    ├── SimpleControl/             # Serial control
    ├── WiFiControl_UnoR4/         # WiFi (Uno R4)
    ├── WiFiControl_ESP32/         # WiFi (ESP32)
    └── BLEControl_ESP32/          # Bluetooth (ESP32)
```

**Two-Layer Design:**
1. **RoombaOI** - Direct protocol implementation for advanced users
2. **ArduRoomba** - Friendly wrapper for common operations

Access the low-level layer anytime via `roomba.getOI()`.

## Quick Start

### Hardware Setup

**Wiring (Arduino Uno R3/R4):**
| Roomba Mini-DIN | Arduino |
|-----------------|---------|
| TX (Pin 4)      | Pin 2   |
| RX (Pin 3)      | Pin 3   |
| DD (Pin 5)      | Pin 4   |
| GND (Pin 6/7)   | GND     |

**Wiring (ESP32):**
| Roomba Mini-DIN | ESP32   |
|-----------------|---------|
| TX (Pin 4)      | GPIO 16 |
| RX (Pin 3)      | GPIO 17 |
| DD (Pin 5)      | GPIO 5  |
| GND (Pin 6/7)   | GND     |

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

void loop() {}
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

  // Create WiFi access point
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

  // Connect to existing WiFi network
  wifi.beginClient("YourSSID", "YourPassword");
  wifi.startWebServer();

  Serial.print("Control at: http://");
  Serial.println(wifi.getIPAddress());
}

void loop() {
  wifi.handleClient();
}
```

### Bluetooth Control (ESP32)

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

## HTTP API Reference

All WiFi implementations expose these endpoints:

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Web control interface |
| `/cmd` | GET | Execute command |
| `/status` | GET | JSON status response |

**Command Parameters:**
```
/cmd?action=forward&speed=200&duration=1000
```
- `action`: forward, backward, left, right, spinLeft, spinRight, stop
- `speed`: 0-500 mm/s
- `duration`: milliseconds (0 = continuous)

**Status Response:**
```json
{
  "voltage": 15800,
  "connected": true,
  "wall": false,
  "bumper": false,
  "remoteConnected": true
}
```

## Supported Hardware

**Microcontrollers:**
- Arduino Uno R3 (basic control only)
- Arduino Uno R4 WiFi (WiFi + basic control)
- Arduino Mega
- ESP32 (WiFi + BLE + basic control)
- ESP8266 (WiFi + basic control)

**Roomba Models:**
- iRobot Create 2
- Roomba 500 series
- Roomba 600 series
- Roomba 700 series

## Installation

### Arduino Library Manager
1. Open Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries
3. Search for "ArduRoomba"
4. Click Install

### Manual Installation
1. Download the latest release from [GitHub](https://github.com/pkyanam/ArduRoomba/releases)
2. Extract to your Arduino libraries folder
3. Restart Arduino IDE

### PlatformIO
```ini
lib_deps = pkyanam/ArduRoomba
```

## Contributing

Contributions are welcome! Whether it's bug fixes, new features, documentation, or examples - we appreciate your help in keeping old robots out of landfills.

## License

MIT License - See [LICENSE](LICENSE) file for details.

Free for commercial and non-commercial use. Build cool things!

---

**Version 3.1.0** - WiFi & Bluetooth wireless control
