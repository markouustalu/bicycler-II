#ifndef NO2PROTOCOL_H
#define NO2PROTOCOL_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Config.h"

// Controller message structure (14-byte format)
struct ControllerMessage {
  //uint8_t address; //not needed, checked before processing
  //uint8_t frameLength; //not needed, checked before processing
  //uint8_t commandNumber; //not needed, checked before processing
  uint8_t controllerStatus1;
  uint8_t controllerStatus2;
  uint16_t runningCurrent;
  int8_t motorTemp;  // Used to be currentRatioValue
  uint16_t speedFeedback;
  //uint8_t batteryCapacity; //Not implemented
  //uint16_t remainingMileage; //Not implemented
  //uint8_t checksum; //not needed, checked before processing
};

// Instrument message structure (20-byte format)
struct InstrumentMessage {
  uint8_t address;
  uint8_t frameLength;
  uint8_t commandNumber;
  uint8_t driveMode;
  uint8_t PASlevel;
  uint8_t controllerSetting;
  uint8_t motorPoleCount;
  uint16_t wheelDiameter;
  uint8_t PASassistanceSensitivity;
  uint8_t PASassistanceStartIntensity;
  uint16_t speedMotorPolePairCountAndSpeedLimit;
  uint8_t controllerCurrentLimit;
  uint16_t controllerUndervoltageValue;
  uint16_t throttleSpeedPwmDutyCycle;
  uint8_t controllerSetting2AndPASDiscMagnets;
  uint8_t checksum;
};

// Controller status bits
#define STATUS1_CRUISE_6KM      0x80
#define STATUS1_HALL_FAULT      0x40
#define STATUS1_THROTTLE_FAULT  0x20
#define STATUS1_CONTROLLER_FAULT 0x10
#define STATUS1_UNDERVOLTAGE    0x08
#define STATUS1_CRUISE          0x04
#define STATUS1_BRAKE_FAULT     0x02
#define STATUS1_PHASE_LOSS      0x01

#define STATUS2_LEVEL           0x80
#define STATUS2_ASSISTANCE_FAULT 0x40
#define STATUS2_BRAKE_ACTIVE    0x20
#define STATUS2_COMM_FAULT      0x10
#define STATUS2_CHARGING        0x08
#define STATUS2_SPEED_LIMIT     0x04

// Controller control bits
#define CONTROLLER_POWER_SWITCH           0x80
#define CONTROLLER_ZERO_START             0x40
#define CONTROLLER_LIGHT_ON               0x20
#define CONTROLLER_COMM_FAULT             0x10
#define CONTROLLER_CRUISE_SWITCH_METHOD   0x08
#define CONTROLLER_SPEED_LIMIT            0x04
#define CONTROLLER_CRUISE_HOLD_MODE       0x02
#define CONTROLLER_CRUISE_SWITCH_MODE     0x01

class Config; // Forward declaration

class NO2Protocol {
public:
  NO2Protocol();
  Config* config;

  bool stateChanged = false;
  bool unlimitedMode = false;
  
  void begin(SoftwareSerial* serial, Config* config);
  void processIncomingMessage();
  void sendInstrumentMessage();
  void loop();
  
  // Getters for display data
  float getSpeedKmh() const { return speedKmh; }
  float getCurrent() const { return current; }
  byte getTemperature() const { return controllerMsg.motorTemp; }
  unsigned long getTripMeter() const { return tripMeter; } // in centimeters
  uint16_t getOdometer() const { return odoMeter; } // in kilometers
  byte getControllerStatus1() const { return controllerMsg.controllerStatus1; }
  byte getControllerStatus2() const { return controllerMsg.controllerStatus2; }
  float getCumulativeAh() const { return cumulativeAh; }
  unsigned long getSessionTimeMs() const { return sessionTimeMs; }
  
  // Configuration setters
  void setDriveMode(uint8_t mode);
  void setAssistanceLevel(uint8_t level);
  void setUnlimitedMode();
  void setCumulativeAh(float ah) { cumulativeAh = ah; }
  void setTripMeter(unsigned long tm) { tripMeter = tm; }
  void setSessionTimeMs(unsigned long t) { sessionTimeMs = t; }
  // void setCruise(bool mode);

  // Configuration getters
  uint8_t getDriveMode() const { return driveMode; }
  uint8_t getAssistanceLevel() const { return PASActualLevel; }

private:
  SoftwareSerial* serial;
  
  ControllerMessage controllerMsg;
  InstrumentMessage instrumentMsg;
  
  // Calculated values
  float speedKmh;
  float current, cumulativeAh;
  unsigned long tripMeter; // in centimeters
  unsigned long lastOdometerUpdateTripMeter = 0;
  uint16_t odoMeter; // in kilometers
  unsigned long sessionTimeMs; // time elapsed since boot
  
  // Configuration
  uint8_t driveMode;
  uint8_t PASRequestedLevel, PASActualLevel;
  uint8_t maxPASLevel;

  // Buffer for incoming message
  uint8_t buffer[14];

  void updateCalculatedValues();
  void initializeInstrumentMessage();
};

#endif