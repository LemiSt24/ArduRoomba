/**
 * WiFiControl_UnoR4.ino
 *
 * WiFi control example for Arduino Uno R4 WiFi
 * Creates a WiFi access point and web server for remote Roomba control
 *
 * Hardware:
 * - Arduino Uno R4 WiFi
 * - iRobot Create 2 or compatible Roomba
 *
 * Connections:
 * - Roomba TX -> Arduino Pin 2 (RX)
 * - Roomba RX -> Arduino Pin 3 (TX)
 * - Roomba DD -> Arduino Pin 4 (BRC)
 * - Common GND
 */

#include "ArduRoomba.h"
#include "extensions/ArduRoombaWiFiS3.h"

// Roomba configuration
ArduRoomba roomba(2, 3, 4);  // RX, TX, BRC pins

// WiFi configuration
const char* AP_SSID = "ArduRoomba";
const char* AP_PASSWORD = "roomba123";  // Set to nullptr for open network

// WiFi extension
ArduRoombaWiFiS3 wifiControl(roomba);

void setup() {
  Serial.begin(19200);
  while (!Serial) delay(10);

  Serial.println("\n=== ArduRoomba WiFi Control ===");
  Serial.println("Arduino Uno R4 WiFi Edition");

  // Initialize Roomba
  Serial.println("\nInitializing Roomba...");
  if (!roomba.begin()) {
    Serial.println("ERROR: Failed to connect to Roomba!");
    Serial.println("Check wiring and power, then reset.");
    while (1) delay(1000);
  }
  Serial.println("Roomba connected!");

  // Start WiFi Access Point
  Serial.println("\nStarting WiFi Access Point...");
  if (!wifiControl.beginAP(AP_SSID, AP_PASSWORD)) {
    Serial.println("ERROR: Failed to create WiFi AP!");
    while (1) delay(1000);
  }

  // Start web server
  wifiControl.startWebServer(80);

  Serial.println("\n=== Setup Complete ===");
  Serial.println("Connect to WiFi network:");
  Serial.print("  SSID: ");
  Serial.println(AP_SSID);
  if (AP_PASSWORD) {
    Serial.print("  Password: ");
    Serial.println(AP_PASSWORD);
  }
  Serial.println("\nThen open browser to:");
  Serial.print("  http://");
  Serial.println(wifiControl.getIPAddress());
  Serial.println("\nReady for commands!");
}

void loop() {
  // Handle incoming web requests
  wifiControl.handleClient();

  // Optional: Add any additional logic here
  // For example, automatic obstacle avoidance:
  /*
  if (roomba.isBumperPressed()) {
    roomba.moveBackward(150);
    delay(500);
    roomba.turnRight(150);
    delay(800);
    roomba.stop();
  }
  */

  delay(10);  // Small delay for stability
}

// Optional: Custom command handler
// Uncomment to add custom behavior for specific commands
/*
void onCommand(const RoombaCommand& cmd) {
  Serial.print("Custom handler - Action: ");
  Serial.print(cmd.action);
  Serial.print(", Speed: ");
  Serial.println(cmd.speed);

  // Add custom logic here
  // For example, log commands, trigger LEDs, etc.
}
*/
