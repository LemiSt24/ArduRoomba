/**
 * @file ArduRoomba.h
 * @brief Main ArduRoomba library interface
 * 
 * Clean, minimal interface for controlling iRobot Create 2 and compatible Roomba models.
 * Provides convenient methods while maintaining modularity for future extensions.
 */

#ifndef ARDUROOMBA_H
#define ARDUROOMBA_H

#include "RoombaOI.h"

class ArduRoomba {
public:
  // Constructor
  ArduRoomba(uint8_t rxPin, uint8_t txPin, uint8_t brcPin);
  
  // Basic lifecycle
  bool begin(uint32_t baudRate = 19200);
  void end();
  bool isConnected() const;
  
  // Simple movement commands
  void moveForward(int16_t speed = 200);
  void moveBackward(int16_t speed = 200);
  void turnLeft(int16_t speed = 200);
  void turnRight(int16_t speed = 200);
  void stop();
  
  // Advanced movement
  void drive(int16_t velocity, int16_t radius);
  void driveDirect(int16_t rightVel, int16_t leftVel);
  
  // Cleaning modes
  void startCleaning();
  void spotClean();
  void dock();
  
  // Basic sensors
  uint16_t getBatteryVoltage();
  int16_t getBatteryCurrent();
  bool isWallDetected();
  bool isBumperPressed();
  
  // Actuators
  void setBrushes(bool main, bool side, bool vacuum = false);
  void setLED(bool debris, bool spot, bool dock, bool checkRobot = false);
  void setPowerLED(uint8_t color, uint8_t intensity = 255);
  
  // Sound
  void beep();
  void playTone(uint8_t note, uint8_t duration);
  
  // Utility
  void setDebug(bool enable);
  
  // Access to underlying OI layer for advanced use
  RoombaOI& getOI() { return _oi; }
  
private:
  RoombaOI _oi;
  bool _debug;
  
  void debugPrint(const char* msg);
  void debugPrint(const char* msg, int value);
};

#endif