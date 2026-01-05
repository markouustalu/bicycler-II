#include "Config.h"

Config::Config() {
  //make one time clearing of eeprom from previous config structure and seed odo with 1717 km
  // for (uint8_t i = 0; i < (sizeof(data) * CONFIG_ARRAY_SIZE); i++) {
  //   EEPROM.write(i, 0x00); 
  // }
  // data.odometer = (uint16_t)1717;
  // save();

  load();
}

void Config::load() {
  // Read configuration from EEPROM with the highest odometer value
  byte curIndex = 0;
  uint16_t odo1, odo2;
  EEPROM.get(0, odo1);
  for (byte i = 1; i < CONFIG_ARRAY_SIZE; i++) {
    EEPROM.get(i * sizeof(ConfigData), odo2);
    if (odo2 > odo1) {
      odo1 = odo2;
      curIndex = i;
    }
  }
  EEPROM.get(curIndex * sizeof(ConfigData), data);
}

void Config::save() {
  //iterate over EEPROM in sizeof(data) chunks to find the chunk with the lowest odometer value
  byte curIndex = 0;
  uint16_t odo1, odo2;
  EEPROM.get(0, odo1);
  for (byte i = 1; i < CONFIG_ARRAY_SIZE; i++) {
    EEPROM.get(i * sizeof(ConfigData), odo2);
    if (odo2 < odo1) {
      odo1 = odo2;
      curIndex = i;
    }
  }
  EEPROM.put(curIndex * sizeof(ConfigData), data);
}

void Config::manualSave() {
  byte curIndex = 0;
  uint16_t odo1, odo2;
  EEPROM.get(0, odo1);
  for (byte i = 1; i < CONFIG_ARRAY_SIZE; i++) {
    EEPROM.get(i * sizeof(ConfigData), odo2);
    if (odo2 >= odo1) {
      odo1 = odo2;
      curIndex = i;
    }
  }
  EEPROM.put(curIndex * sizeof(ConfigData), data);
}

void Config::setOdometer(uint16_t odoMeter) {
  data.odometer = odoMeter;
  save();
}

uint16_t Config::getOdometer() {
  return data.odometer;
}

float Config::getCumulativeAh() const {
  return data.cumulativeAh / 100.0f;
}

unsigned long Config::getTripMeter() const {
  return data.tripMeter * 1000.0f;
}

unsigned long Config::getSessionTimeMs() const {
  return data.sessionTimeS * 1000;
}

void Config::setCumulativeAh(float ah) {
  // Store cumulative Ah with two decimal precision
  data.cumulativeAh = ah * 100;
}

void Config::setTripMeter(unsigned long tm) {
  // Store trip meter in 10 meter units
  data.tripMeter = tm / 1000;
}

void Config::setSessionTimeMs(unsigned long t) {
  // Store session time in seconds
  data.sessionTimeS = t / 1000;
}
