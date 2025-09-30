#include "Config.h"

Config::Config() : eepromAddress(0) {
  load();
}

void Config::load() {
  // Read configuration from EEPROM
  EEPROM.get(eepromAddress, data); 
}

void Config::save() {
  EEPROM.put(eepromAddress, data);
}

void Config::setWheelDiameter(uint16_t diameter) {
  data.wheelDiameter = diameter;
  save();
}

void Config::setOdometer(uint16_t odoMeter) {
  //Iterate over odometer array and replace the smallest value
  int minIndex = 0;
  for (int i = 1; i < 10; i++) {
    if (data.odometerArray[i] < data.odometerArray[minIndex]) {
      minIndex = i;
    }
  }
  data.odometerArray[minIndex] = odoMeter;
  save();
}

uint16_t Config::getOdometer() const {
  //Return the maximum value from the odometer array
  uint16_t maxOdo = 0;
  for (int i = 0; i < 10; i++) {
    if (data.odometerArray[i] > maxOdo) {
      maxOdo = data.odometerArray[i];
    }
  }
  return maxOdo;
}

