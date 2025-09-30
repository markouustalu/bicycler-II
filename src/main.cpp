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
    // Initialize controller communication
    controllerSerial.begin(9600);
    
    // Initialize buttons
    buttons.init();

    // Initialize display
    display.init();
   
    // Initialize protocol
    protocol.begin(&controllerSerial, &config);
}

void loop() {
    // Handle button presses
    buttons.loop();
   
    // Handle protocol communication
    protocol.loop();
   
    // Display update
    display.loop();
}