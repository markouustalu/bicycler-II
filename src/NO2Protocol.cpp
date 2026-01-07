#include "NO2Protocol.h"

NO2Protocol::NO2Protocol() : 
  serial(nullptr),
  config(nullptr),
  speedKmh(0.0f),
  current(0.0f),
  tripMeter(0),
  driveMode(0),
  PASRequestedLevel(5),
  PASActualLevel(5),
  maxPASLevel(5),
  cumulativeAh(0.0f),
  sessionTimeMs(0)
{
  // Initialize message structures to zero
  memset(&controllerMsg, 0, sizeof(ControllerMessage));
  memset(&instrumentMsg, 0, sizeof(InstrumentMessage));
}

void NO2Protocol::begin(SoftwareSerial* serial, Config* config) {
  this->serial = serial;
  this->config = config;
  odoMeter = config->getOdometer();
  cumulativeAh = config->getCumulativeAh();
  tripMeter = config->getTripMeter();
  sessionTimeMs = config->getSessionTimeMs();
  lastOdometerUpdateTripMeter = tripMeter - (tripMeter % 100000);
  initializeInstrumentMessage();
}

void NO2Protocol::loop() {
  
  // Handle controller communication
  static unsigned long sendNextInstrumentMessage = 0;
  while(serial->available()) {

    //Read byte by byte into buffer and push data in like a stack
    for (int i = 0; i < 13; i++) {
      buffer[i] = buffer[i + 1];
    }
    buffer[13] = serial->read();

    // Verify frame structure
    if (buffer[0] == 0x02 && buffer[1] == 14 && buffer[2] == 0x01) {

      // Calculate checksum (XOR all bytes including checksum)
      uint8_t calculatedChecksum = 0;
      for (int i = 0; i < 14; i++) {
        calculatedChecksum ^= buffer[i];
      }
      
      // Verify checksum
      if (calculatedChecksum == 0) {
        processIncomingMessage();  
        sendNextInstrumentMessage = millis() + 15; // Schedule next message send in 15ms
      }
    } 
  }

  // Send response after 15ms of last processed message
  if (millis() > sendNextInstrumentMessage) {
      sendInstrumentMessage();
      sendNextInstrumentMessage = millis() + 500; // Avoid sending repeatedly
  }
}

void NO2Protocol::processIncomingMessage() {
  // Parse message
  controllerMsg.controllerStatus1 = buffer[3];
  controllerMsg.controllerStatus2 = buffer[4];
  controllerMsg.runningCurrent = (buffer[5] << 8) | buffer[6];
  controllerMsg.motorTemp = buffer[7] - 40; // Offset by 40
  controllerMsg.speedFeedback = (buffer[8] << 8) | buffer[9];
  
  // Update calculated values
  updateCalculatedValues();
  
  stateChanged = true;
}

void NO2Protocol::updateCalculatedValues() {
  // Calculate speed in km/h
  speedKmh = 0.0f;
  static const float wheelCircumferenceMeters = (instrumentMsg.wheelDiameter / 10.0f) * 0.0254f * PI * 3.6f;
  static unsigned long lastMeasurementTime = 0;
  
  if (controllerMsg.speedFeedback > 0 && controllerMsg.speedFeedback < 0x7530) {
    // Calculate speed in km/h
    speedKmh = wheelCircumferenceMeters * (1000.0f / (controllerMsg.speedFeedback * (32 / instrumentMsg.motorPoleCount)));
  }

  // Maximum allowed speed based on PAS level follows the following formula:
  // maxAllowedSpeed = ((PASLevel - 1) * 5) + 2
  // For each kmh over the allowe speed, reduce the PAS level by 1
  // The controller overshoots when reaching target speed, so we have to reduce PAS level to reduce overshoot
  uint8_t maxAllowedSpeed = ((PASRequestedLevel - 1) * 5) + 2;
  static bool speedLimitTripped = false;
  if (unlimitedMode) {
    if (speedKmh > maxAllowedSpeed) {
      if(speedKmh - maxAllowedSpeed > PASRequestedLevel - 1) {
        PASActualLevel = 1;
      } else {
        PASActualLevel = PASRequestedLevel - (uint8_t)(speedKmh - maxAllowedSpeed);
      }
    } else {
      PASActualLevel = PASRequestedLevel;
    }
  } else {
    //implement hard speed limiting with hysteresis. 25 is hard limit.
    if (!speedLimitTripped && speedKmh < 25) {
      PASActualLevel = PASRequestedLevel;
    } else if (!speedLimitTripped && speedKmh >= 25) {
      speedLimitTripped = true;
      PASActualLevel = 0;
    } else if (speedLimitTripped && speedKmh < 23) {
      speedLimitTripped = false;
      PASActualLevel = PASRequestedLevel;
    }
  }

  // Update trip meter (in cm)
  tripMeter += (unsigned long)(speedKmh * 100.0f / 3600.0f * (millis() - lastMeasurementTime));
  config->setTripMeter(tripMeter);

  if (tripMeter - lastOdometerUpdateTripMeter >= 100000) {
    lastOdometerUpdateTripMeter = tripMeter - (tripMeter % 100000);
    odoMeter += 1;
    config->setOdometer(odoMeter);
  }
  
  // Calculate current and cumulative Ah
  uint8_t currentUnit = (controllerMsg.runningCurrent >> 8) & 0x40;
  current = (float)(controllerMsg.runningCurrent & 0xFF) / (currentUnit ? 10.0f : 1.0f);
  if (controllerMsg.controllerStatus2 & STATUS2_BRAKE_ACTIVE) {
    cumulativeAh -= current * (millis() - lastMeasurementTime) / 3600000.0f; // Ah (regenerative braking)
  } else {
    cumulativeAh += current * (millis() - lastMeasurementTime) / 3600000.0f; // Ah
  }
  config->setCumulativeAh(cumulativeAh);
  if (!unlimitedMode && current > 5) {
    current = 5;
  }

  sessionTimeMs += (millis() - lastMeasurementTime);
  config->setSessionTimeMs(sessionTimeMs);
  lastMeasurementTime = millis();
}

void NO2Protocol::sendInstrumentMessage() {
  // Update instrument message with current settings
  instrumentMsg.driveMode = driveMode;
  instrumentMsg.PASlevel = PASActualLevel;
  
   // Calculate checksum
  instrumentMsg.checksum = 0;
  for (uint8_t i = 0; i < 19; i++) {
    instrumentMsg.checksum ^= ((uint8_t*)&instrumentMsg)[i];
  }
  
  // Send message via serial
  serial->write((uint8_t*)&instrumentMsg, 20);
}

void NO2Protocol::initializeInstrumentMessage() {
  // Set fixed fields
  instrumentMsg.address = 0x01;
  instrumentMsg.frameLength = 20;
  instrumentMsg.commandNumber = 0x01;
  
  //TODO: set better default values based on what is actually required for my controller
  // Set default values according to protocol
  instrumentMsg.driveMode = driveMode;
  instrumentMsg.PASlevel = PASActualLevel;
  instrumentMsg.controllerSetting = CONTROLLER_POWER_SWITCH; // Controller on, zero start, light on, no fault
  instrumentMsg.motorPoleCount = 8; 
  instrumentMsg.wheelDiameter = 260; // 26 inches
  instrumentMsg.PASassistanceSensitivity = 1; // Hall sensor sensitivity
  instrumentMsg.PASassistanceStartIntensity = 0; // Default start intensity
  instrumentMsg.speedMotorPolePairCountAndSpeedLimit = 0x2019; // No speed limit
  instrumentMsg.controllerCurrentLimit = 12; // 12A default
  instrumentMsg.controllerUndervoltageValue = 480; // 48.0V default
  instrumentMsg.throttleSpeedPwmDutyCycle = 0000; // 0% default
  instrumentMsg.controllerSetting2AndPASDiscMagnets = 0x08; // No reverse, no auto cruise
}

// Configuration setters
void NO2Protocol::setDriveMode(uint8_t mode) {
  if (mode <= 2 && mode >= 0) {
    driveMode = mode;
  }
}

void NO2Protocol::setAssistanceLevel(uint8_t level) {
  if (level <= maxPASLevel && level >= 0) {
    PASRequestedLevel = level;
  }
}

void NO2Protocol::setUnlimitedMode() {
  setDriveMode(2); // Electric drive
  maxPASLevel = 15;
  setAssistanceLevel(6);
  unlimitedMode = true;
}