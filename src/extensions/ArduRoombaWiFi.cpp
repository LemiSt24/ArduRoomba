/**
 * @file ArduRoombaWiFi.cpp
 * @brief Implementation of base WiFi functionality
 */

#include "ArduRoombaWiFi.h"

ArduRoombaWiFi::ArduRoombaWiFi(ArduRoomba& roomba)
  : _roomba(roomba), _remoteEnabled(true), _commandCallback(nullptr) {
}

void ArduRoombaWiFi::processCommand(const RoombaCommand& cmd) {
  if (!_remoteEnabled) return;

  // Call user callback if set
  if (_commandCallback) {
    _commandCallback(cmd);
  }

  // Process standard commands
  String action = String(cmd.action);

  if (action == "forward") {
    _roomba.moveForward(cmd.speed > 0 ? cmd.speed : 200);
  }
  else if (action == "backward") {
    _roomba.moveBackward(cmd.speed > 0 ? cmd.speed : 200);
  }
  else if (action == "left") {
    _roomba.turnLeft(cmd.speed > 0 ? cmd.speed : 150);
  }
  else if (action == "right") {
    _roomba.turnRight(cmd.speed > 0 ? cmd.speed : 150);
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
  if (cmd.duration > 0 && action != "stop") {
    delay(cmd.duration);
    _roomba.stop();
  }
}

void ArduRoombaWiFi::setCommandCallback(void (*callback)(const RoombaCommand&)) {
  _commandCallback = callback;
}

String ArduRoombaWiFi::generateControlPage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ArduRoomba Control</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      background: #2c3e50;
      color: #ecf0f1;
      padding: 20px;
    }
    h1 { color: #3498db; }
    .controls {
      display: grid;
      grid-template-columns: repeat(3, 100px);
      gap: 10px;
      justify-content: center;
      margin: 20px auto;
    }
    button {
      padding: 20px;
      font-size: 16px;
      background: #3498db;
      color: white;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      transition: 0.3s;
    }
    button:active { background: #2980b9; transform: scale(0.95); }
    .forward { grid-column: 2; }
    .left { grid-column: 1; grid-row: 2; }
    .stop { grid-column: 2; grid-row: 2; background: #e74c3c; }
    .right { grid-column: 3; grid-row: 2; }
    .backward { grid-column: 2; grid-row: 3; }
    .actions { margin-top: 30px; }
    .actions button { margin: 5px; background: #27ae60; }
    .status { margin: 20px; padding: 15px; background: #34495e; border-radius: 8px; }
  </style>
</head>
<body>
  <h1>ArduRoomba Control</h1>
  <div class="status" id="status">Battery: -- mV | Status: --</div>
  <div class="controls">
    <button class="forward" onclick="send('forward')">↑</button>
    <button class="left" onclick="send('left')">←</button>
    <button class="stop" onclick="send('stop')">STOP</button>
    <button class="right" onclick="send('right')">→</button>
    <button class="backward" onclick="send('backward')">↓</button>
  </div>
  <div class="actions">
    <button onclick="send('clean')">Clean</button>
    <button onclick="send('spot')">Spot Clean</button>
    <button onclick="send('dock')">Dock</button>
    <button onclick="send('beep')">Beep</button>
  </div>
  <script>
    function send(action) {
      fetch('/cmd?action=' + action)
        .then(r => r.text())
        .then(t => console.log(t))
        .catch(e => console.error(e));
    }
    function updateStatus() {
      fetch('/status')
        .then(r => r.json())
        .then(d => {
          document.getElementById('status').innerHTML =
            'Battery: ' + d.voltage + ' mV | Connected: ' + d.connected;
        })
        .catch(e => console.error(e));
    }
    setInterval(updateStatus, 2000);
    updateStatus();
  </script>
</body>
</html>
)rawliteral";
  return html;
}

String ArduRoombaWiFi::generateStatusJSON() {
  uint16_t voltage = _roomba.getBatteryVoltage();
  bool connected = _roomba.isConnected();

  String json = "{";
  json += "\"voltage\":" + String(voltage) + ",";
  json += "\"connected\":" + String(connected ? "true" : "false") + ",";
  json += "\"remote_enabled\":" + String(_remoteEnabled ? "true" : "false");
  json += "}";

  return json;
}

void ArduRoombaWiFi::startWebServer(uint16_t port) {
  // Implemented by platform-specific class
}

void ArduRoombaWiFi::handleClient() {
  // Implemented by platform-specific class
}
