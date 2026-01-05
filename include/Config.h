#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <EEPROM.h>

#define CONFIG_ARRAY_SIZE 10

// Configuration structure
struct ConfigData {
  //odometer is updates every km, so implement some wear leveling over the array of 10 elements
  //to avoid wearing out a single eeprom cell
  uint16_t odometer;
  uint16_t cumulativeAh;
  uint16_t tripMeter; // in 10 meter units
  uint16_t sessionTimeS; // time elapsed since boot in S
};

class Config {
public:
  Config();
  
  void load();
  void save();
  void manualSave();
  
  // Getters
  uint16_t getOdometer();
  float getCumulativeAh() const;
  unsigned long getTripMeter() const;
  unsigned long getSessionTimeMs() const;
  
  // Setters
  void setOdometer(uint16_t odoMeter);
  void setCumulativeAh(float ah);
  void setTripMeter(unsigned long tm);
  void setSessionTimeMs(unsigned long t);
  
private:
  ConfigData data;
};

#endif