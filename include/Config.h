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
  void initializeEEPROM();
  
  // Getters
  uint16_t getOdometer() const { return data.odometer; }
  float getCumulativeAh() const { return cumulativeAh; }
  unsigned long getTripMeter() const { return tripMeter; }
  unsigned long getSessionTimeMs() const { return sessionTimeMs; }
  
  // Setters
  void setOdometer(uint16_t odoMeter);
  void setCumulativeAh(float ah) { cumulativeAh = ah; }
  void setTripMeter(unsigned long tm) { tripMeter = tm; }
  void setSessionTimeMs(unsigned long t) { sessionTimeMs = t; }

private:
  ConfigData data;
  float cumulativeAh;
  unsigned long tripMeter; // in centimeters
  unsigned long sessionTimeMs; // time elapsed since boot

};

#endif