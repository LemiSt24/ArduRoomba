/**
 * WiFiControl_ESP32.ino
 *
 * WiFi control example for ESP32 boards
 * Creates a WiFi access point and web server for remote Roomba control
 *
 * Hardware:
 * - ESP32 board (ESP32-DevKitC, ESP32-WROOM, etc.)
 * - iRobot Create 2 or compatible Roomba
 *
 * Connections:
 * - Roomba TX -> ESP32 GPIO 16 (RX2)
 * - Roomba RX -> ESP32 GPIO 17 (TX2)
 * - Roomba DD -> ESP32 GPIO 5 (BRC)
 * - Common GND
 *
 * Note: This example uses Hardware Serial 2 for better performance
 * Modify ArduRoomba to use HardwareSerial if needed, or adjust pins for SoftwareSerial
 */

#include "ArduRoomba.h"
#include "extensions/ArduRoombaESP32WiFi.h"

// Roomba configuration
// For ESP32, you might want to use different pins or HardwareSerial
ArduRoomba roomba(16, 17, 5);  // RX, TX, BRC pins

// WiFi configuration - Choose AP or Client mode

// Option 1: Access Point mode (Roomba creates its own network)
#define USE_AP_MODE true
const char* AP_SSID = "ArduRoomba-ESP32";
const char* AP_PASSWORD = "roomba123";  // Set to nullptr for open network

// Option 2: Client mode (Roomba connects to your existing WiFi)
// #define USE_AP_MODE false
// const char* WIFI_SSID = "YourWiFiNetwork";
// const char* WIFI_PASSWORD = "YourPassword";

// WiFi extension
ArduRoombaESP32WiFi wifiControl(roomba);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== ArduRoomba WiFi Control ===");
  Serial.println("ESP32 Edition");

  // Initialize Roomba
  Serial.println("\nInitializing Roomba...");
  if (!roomba.begin()) {
    Serial.println("ERROR: Failed to connect to Roomba!");
    Serial.println("Check wiring and power, then reset.");
    while (1) delay(1000);
  }
  Serial.println("Roomba connected!");

  // Start WiFi
  Serial.println("\nStarting WiFi...");

#if USE_AP_MODE
  // Access Point mode
  if (!wifiControl.beginAP(AP_SSID, AP_PASSWORD)) {
    Serial.println("ERROR: Failed to create WiFi AP!");
    while (1) delay(1000);
  }
  Serial.println("WiFi AP created!");
#else
  // Client mode
  if (!wifiControl.beginClient(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("ERROR: Failed to connect to WiFi!");
    while (1) delay(1000);
  }
  Serial.println("WiFi connected!");
#endif

  // Start web server
  wifiControl.startWebServer(80);

  Serial.println("\n=== Setup Complete ===");
#if USE_AP_MODE
  Serial.println("Connect to WiFi network:");
  Serial.print("  SSID: ");
  Serial.println(AP_SSID);
  if (AP_PASSWORD) {
    Serial.print("  Password: ");
    Serial.println(AP_PASSWORD);
  }
#endif
  Serial.println("\nOpen browser to:");
  Serial.print("  http://");
  Serial.println(wifiControl.getIPAddress());
  Serial.println("\nReady for commands!");
}

void loop() {
  // Handle incoming web requests
  wifiControl.handleClient();

  // Optional: Add sensor monitoring and automatic behaviors
  static unsigned long lastSensorCheck = 0;
  if (millis() - lastSensorCheck > 1000) {
    lastSensorCheck = millis();

    // Check battery status
    uint16_t voltage = roomba.getBatteryVoltage();
    if (voltage > 0 && voltage < 13000) {
      Serial.println("Warning: Battery low!");
    }

    // Optional: Automatic obstacle avoidance when remote control is disabled
    /*
    if (!wifiControl.isRemoteEnabled()) {
      if (roomba.isBumperPressed()) {
        Serial.println("Bumper hit - automatic avoidance");
        roomba.moveBackward(150);
        delay(500);
        roomba.turnRight(150);
        delay(800);
        roomba.stop();
      }
    }
    */
  }
}

// Optional: Custom command handler
// Uncomment and register with wifiControl.setCommandCallback(onCommand) in setup()
/*
void onCommand(const RoombaCommand& cmd) {
  Serial.print("Custom handler - Action: ");
  Serial.print(cmd.action);
  Serial.print(", Speed: ");
  Serial.print(cmd.speed);
  Serial.print(", Duration: ");
  Serial.println(cmd.duration);

  // Add custom logic here
  // For example: logging, analytics, trigger external devices, etc.

  // Example: Blink LED on command
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
}
*/
