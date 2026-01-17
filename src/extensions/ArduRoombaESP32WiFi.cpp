/**
 * @file ArduRoombaESP32WiFi.cpp
 * @brief Implementation of WiFi control for ESP32
 */

#include "ArduRoombaESP32WiFi.h"

#if defined(ESP32)

ArduRoombaESP32WiFi::ArduRoombaESP32WiFi(ArduRoomba& roomba)
  : ArduRoombaWiFi(roomba), _server(nullptr), _mode(AR_WIFI_MODE_AP), _connected(false) {
}

ArduRoombaESP32WiFi::~ArduRoombaESP32WiFi() {
  if (_server) {
    delete _server;
  }
}

bool ArduRoombaESP32WiFi::beginAP(const char* ssid, const char* password) {
  Serial.print("Creating WiFi AP: ");
  Serial.println(ssid);

  _mode = AR_WIFI_MODE_AP;

  // Create access point
  bool success;
  if (password && strlen(password) > 0) {
    success = WiFi.softAP(ssid, password);
  } else {
    success = WiFi.softAP(ssid);
  }

  if (!success) {
    Serial.println("Failed to create AP");
    return false;
  }

  delay(100);

  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);

  _connected = true;
  return true;
}

bool ArduRoombaESP32WiFi::beginClient(const char* ssid, const char* password) {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  _mode = AR_WIFI_MODE_CLIENT;

  // Connect to WiFi
  WiFi.begin(ssid, password);

  // Wait for connection
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to WiFi");
    return false;
  }

  IPAddress ip = WiFi.localIP();
  Serial.print("Connected! IP address: ");
  Serial.println(ip);

  _connected = true;
  return true;
}

void ArduRoombaESP32WiFi::end() {
  if (_server) {
    _server->stop();
    delete _server;
    _server = nullptr;
  }

  if (_mode == AR_WIFI_MODE_AP) {
    WiFi.softAPdisconnect(true);
  } else {
    WiFi.disconnect(true);
  }

  _connected = false;
}

bool ArduRoombaESP32WiFi::isConnected() const {
  if (_mode == AR_WIFI_MODE_CLIENT) {
    return WiFi.status() == WL_CONNECTED;
  }
  return _connected;
}

String ArduRoombaESP32WiFi::getIPAddress() const {
  if (_mode == AR_WIFI_MODE_AP) {
    return WiFi.softAPIP().toString();
  }
  return WiFi.localIP().toString();
}

void ArduRoombaESP32WiFi::startWebServer(uint16_t port) {
  if (_server) {
    _server->stop();
    delete _server;
  }

  _server = new WebServer(port);

  // Setup routes
  _server->on("/", [this]() { handleRoot(); });
  _server->on("/cmd", [this]() { handleCommand(); });
  _server->on("/status", [this]() { handleStatus(); });
  _server->onNotFound([this]() { handleNotFound(); });

  _server->begin();

  Serial.print("Web server started on port ");
  Serial.println(port);
  Serial.print("Access at: http://");
  Serial.println(getIPAddress());
}

void ArduRoombaESP32WiFi::handleClient() {
  if (_server) {
    _server->handleClient();
  }
}

void ArduRoombaESP32WiFi::handleRoot() {
  String html = generateControlPage();
  _server->send(200, "text/html", html);
}

void ArduRoombaESP32WiFi::handleCommand() {
  // Parse command parameters
  String action = _server->arg("action");
  String speedStr = _server->arg("speed");
  String durationStr = _server->arg("duration");

  RoombaCommand cmd;
  strncpy(cmd.action, action.c_str(), sizeof(cmd.action) - 1);
  cmd.action[sizeof(cmd.action) - 1] = '\0';
  cmd.speed = speedStr.length() > 0 ? speedStr.toInt() : 200;
  cmd.duration = durationStr.length() > 0 ? durationStr.toInt() : 0;

  processCommand(cmd);

  _server->sendHeader("Access-Control-Allow-Origin", "*");
  _server->send(200, "text/plain", "OK");
}

void ArduRoombaESP32WiFi::handleStatus() {
  String json = generateStatusJSON();
  _server->sendHeader("Access-Control-Allow-Origin", "*");
  _server->send(200, "application/json", json);
}

void ArduRoombaESP32WiFi::handleNotFound() {
  _server->send(404, "text/plain", "Not Found");
}

#endif // ESP32
