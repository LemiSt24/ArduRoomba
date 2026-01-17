/**
 * @file ArduRoombaWiFi.h
 * @brief Base WiFi extension interface for ArduRoomba
 *
 * Provides common WiFi control functionality with platform-specific implementations.
 * Supports both AP mode (Roomba creates hotspot) and Client mode (connects to network).
 */

#ifndef ARDUROOMBA_WIFI_H
#define ARDUROOMBA_WIFI_H

#include "../ArduRoomba.h"

// WiFi operating modes
enum WiFiMode {
  AR_WIFI_MODE_AP,      // Access Point - Roomba creates its own network
  AR_WIFI_MODE_CLIENT   // Client - Roomba connects to existing network
};

// Command protocol for WiFi control
struct RoombaCommand {
  char action[16];   // "forward", "backward", "left", "right", "stop", "clean", "dock"
  int16_t speed;     // Speed parameter (0-500)
  int16_t duration;  // Duration in milliseconds (0 = continuous)
};

/**
 * Base class for WiFi-enabled Roomba control
 * Platform-specific implementations inherit from this
 */
class ArduRoombaWiFi {
public:
  ArduRoombaWiFi(ArduRoomba& roomba);
  virtual ~ArduRoombaWiFi() {}

  // WiFi setup - must be implemented by platform-specific class
  virtual bool beginAP(const char* ssid, const char* password = nullptr) = 0;
  virtual bool beginClient(const char* ssid, const char* password) = 0;
  virtual void end() = 0;

  // Status
  virtual bool isConnected() const = 0;
  virtual String getIPAddress() const = 0;

  // HTTP server control
  virtual void startWebServer(uint16_t port = 80);
  virtual void handleClient();

  // Command processing
  void processCommand(const RoombaCommand& cmd);
  void setCommandCallback(void (*callback)(const RoombaCommand&));

  // Enable/disable remote control
  void enableRemoteControl(bool enable) { _remoteEnabled = enable; }
  bool isRemoteEnabled() const { return _remoteEnabled; }

protected:
  ArduRoomba& _roomba;
  bool _remoteEnabled;
  void (*_commandCallback)(const RoombaCommand&);

  // Helper to generate HTML control page
  String generateControlPage();

  // Helper to generate JSON status
  String generateStatusJSON();
};

#endif
