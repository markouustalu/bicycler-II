#include <Arduino.h>
#include <SoftwareSerial.h>
#include "NO2Protocol.h"
#include "Display.h"
#include "Buttons.h"
#include <U8g2lib.h>
//#include <MUIU8g2.h> //planned to be used to change wheel diameter via menu
#include "Config.h"

// Global objects
SoftwareSerial controllerSerial(7, 9); // RX, TX
U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI u8g2(U8G2_R1, /* cs=*/ 10, /* dc=*/ 6, /* reset=*/ 8);  
NO2Protocol protocol;
Display display(&u8g2, &protocol);
Buttons buttons(&protocol);
Config config;

void setup() {
    //Serial.begin(115200);
    //Serial.println("Bicycler-II starting up...");

    // Initialize controller communication
    controllerSerial.begin(9600);
    //Serial.println("Controller serial initialized.");
    
    // Initialize buttons
    buttons.init();
    //Serial.println("Buttons initialized.");

    // Initialize display
    display.init();
    //Serial.println("Display initialized.");

    // Initialize protocol
    protocol.begin(&controllerSerial, &config);
    //Serial.println("Protocol initialized.");

}

void loop() {
    // Handle button presses
    buttons.loop();
    //Serial.println("Buttons loop executed.");

    // Handle protocol communication
    protocol.loop();
    //Serial.println("Protocol loop executed.");

    // Display update
    display.loop();
    //Serial.println("Display loop executed.");
}