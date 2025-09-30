#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "NO2Protocol.h"

class Display {
public:
    Display(U8G2* display, NO2Protocol* protocol);
    
    void init();
    void loop();
    
private:
    U8G2* u8g2;
    NO2Protocol* protocol;
};

#endif