/**
 * BLEControl_ESP32.ino
 *
 * Bluetooth Low Energy control example for ESP32
 * Creates a BLE GATT server for mobile app control
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
 * BLE Service Info:
 * - Service UUID: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
 * - Command Characteristic (Write): beb5483e-36e1-4688-b7f5-ea07361b26a8
 * - Status Characteristic (Read/Notify): beb5483f-36e1-4688-b7f5-ea07361b26a8
 *
 * Command Format: "action:speed:duration"
 * Examples:
 *   - "forward:200:0" - Move forward at 200mm/s continuously
 *   - "left:150:1000" - Turn left at 150mm/s for 1 second
 *   - "stop:0:0" - Stop
 *   - "clean:0:0" - Start cleaning mode
 *   - "dock:0:0" - Return to dock
 *
 * Status Format: "voltage:connected:wall:bumper:remote"
 * Example: "15800:1:0:0:1" (15800mV, connected, no wall, no bumper, remote enabled)
 *
 * Mobile App Suggestions:
 * - nRF Connect (Android/iOS) - For testing and development
 * - LightBlue (iOS) - BLE explorer and testing
 * - BLE Scanner (Android) - BLE device scanner
 * - Custom app using Flutter Blue, React Native BLE, or native BLE APIs
 */

#include "ArduRoomba.h"
#include "extensions/ArduRoombaBLE.h"

// Roomba configuration
ArduRoomba roomba(16, 17, 5);  // RX, TX, BRC pins

// BLE configuration
const char* BLE_DEVICE_NAME = "ArduRoomba";

// BLE extension
ArduRoombaBLE bleControl(roomba, BLE_DEVICE_NAME);

// LED indicator
const int LED_PIN = 2;  // Built-in LED on most ESP32 boards

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);

  Serial.println("\n=== ArduRoomba BLE Control ===");
  Serial.println("ESP32 Edition");

  // Initialize Roomba
  Serial.println("\nInitializing Roomba...");
  if (!roomba.begin()) {
    Serial.println("ERROR: Failed to connect to Roomba!");
    Serial.println("Check wiring and power, then reset.");
    while (1) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      delay(200);
    }
  }
  Serial.println("Roomba connected!");
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);

  // Initialize BLE
  Serial.println("\nStarting BLE service...");
  if (!bleControl.begin()) {
    Serial.println("ERROR: Failed to initialize BLE!");
    while (1) delay(1000);
  }

  // Optional: Set custom command callback
  // bleControl.setCommandCallback(onBLECommand);

  Serial.println("\n=== Setup Complete ===");
  Serial.println("BLE Device Information:");
  Serial.print("  Name: ");
  Serial.println(BLE_DEVICE_NAME);
  Serial.println("  Service UUID: 4fafc201-1fb5-459e-8fcc-c5c9c331914b");
  Serial.println("\nUse a BLE app to connect:");
  Serial.println("  - nRF Connect (Android/iOS)");
  Serial.println("  - LightBlue (iOS)");
  Serial.println("  - BLE Scanner (Android)");
  Serial.println("\nWaiting for BLE connections...");
  Serial.println("LED will blink when client is connected.");
}

void loop() {
  // Update BLE status (sends notifications to connected clients)
  bleControl.updateStatus();

  // LED indicator for connection status
  static unsigned long lastBlink = 0;
  static bool ledState = false;

  if (bleControl.isConnected()) {
    // Fast blink when connected
    if (millis() - lastBlink > 250) {
      lastBlink = millis();
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
  } else {
    // Slow blink when waiting for connection
    if (millis() - lastBlink > 1000) {
      lastBlink = millis();
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
  }

  // Monitor Roomba status
  static unsigned long lastStatusPrint = 0;
  if (millis() - lastStatusPrint > 5000) {
    lastStatusPrint = millis();

    Serial.println("\n--- Status ---");
    Serial.print("BLE Connected: ");
    Serial.println(bleControl.isConnected() ? "Yes" : "No");
    Serial.print("Connection Count: ");
    Serial.println(bleControl.getConnectionCount());
    Serial.print("Battery Voltage: ");
    Serial.print(roomba.getBatteryVoltage());
    Serial.println(" mV");
    Serial.print("Wall Detected: ");
    Serial.println(roomba.isWallDetected() ? "Yes" : "No");
    Serial.print("Bumper Pressed: ");
    Serial.println(roomba.isBumperPressed() ? "Yes" : "No");
  }

  // Optional: Automatic safety features
  /*
  // Auto-stop on low battery
  uint16_t voltage = roomba.getBatteryVoltage();
  if (voltage > 0 && voltage < 13000) {
    Serial.println("CRITICAL: Battery very low, stopping...");
    roomba.stop();
    bleControl.enableRemoteControl(false);  // Disable remote commands
    delay(1000);
  }

  // Automatic obstacle avoidance when not controlled remotely
  if (!bleControl.isConnected()) {
    if (roomba.isBumperPressed()) {
      roomba.moveBackward(150);
      delay(500);
      roomba.turnRight(150);
      delay(800);
      roomba.stop();
    }
  }
  */

  delay(10);
}

// Optional: Custom BLE command handler
// Uncomment and register in setup() with bleControl.setCommandCallback(onBLECommand)
/*
void onBLECommand(const String& command) {
  Serial.print("BLE Command received: ");
  Serial.println(command);

  // Parse command
  int firstColon = command.indexOf(':');
  String action = command.substring(0, firstColon);

  // Add custom logic here
  // Examples:
  // - Log commands to SD card
  // - Trigger external relays or devices
  // - Send notifications
  // - Implement custom behaviors

  // Visual feedback
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(50);
  }
}
*/
