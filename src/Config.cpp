#include "Config.h"

Config::Config() {
  load();
  //for one time only, initialize EEPROM with default values
  //initializeEEPROM();
}

//initialize EEPROM with zeroes and 1750km in odometer
void Config::initializeEEPROM() {
  ConfigData initData = {1750, 0, 0, 0};
  for (byte i = 0; i < CONFIG_ARRAY_SIZE; i++) {
    EEPROM.put(i * sizeof(ConfigData), initData);
  }
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

  cumulativeAh = data.cumulativeAh / 100.0f;
  tripMeter = data.tripMeter * 1000.0f;
  sessionTimeMs = data.sessionTimeS * 1000L;
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

  data.cumulativeAh = cumulativeAh * 100;
  data.tripMeter = tripMeter / 1000;
  data.sessionTimeS = sessionTimeMs / 1000;

  EEPROM.put(curIndex * sizeof(ConfigData), data);
}

void Config::setOdometer(uint16_t odoMeter) {
  data.odometer = odoMeter;
  save();
}