#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <EEPROM.h>

// Configuration structure
struct ConfigData {
  //odometer is updates every km, so implement some wear leveling over the array of 10 elements
  //to avoid wearing out a single eeprom cell
  uint16_t odometerArray[10];
  uint16_t wheelDiameter;       // 0-255 (represents 10-35 inches in 0.1 increments)
};

class Config {
public:
  Config();
  
  void load();
  void save();
  
  // Getters
  uint8_t getWheelDiameter() const { return data.wheelDiameter; }
  uint16_t getOdometer() const;
  
  // Setters
  void setWheelDiameter(uint16_t diameter);
  void setOdometer(uint16_t odoMeter);
  
private:
  ConfigData data;
  uint8_t eepromAddress;

};

#endif