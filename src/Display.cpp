#include "Display.h"

Display::Display(U8G2* display, NO2Protocol* protocol) 
    : u8g2(display), protocol(protocol) {}

void Display::init() {
    u8g2->begin();
    u8g2->setContrast(255); // Max contrast
}

void Display::loop() {
    // static bool updatingDisplay = false;

    //print the state of protocol->stateChanged
    //Serial.print("Protocol stateChanged: ");
    //Serial.println(protocol->stateChanged);

    //return; // Temporarily disable display updates
    static unsigned long lastSpeedNotZero = 0;
    static byte updateState = 0;
    uint16_t averageSpeed = 0;
    // if (!updatingDisplay && protocol->stateChanged) {
    if (!updateState && protocol->stateChanged) {
        updateState++;
        protocol->stateChanged = false; // Reset state change flag
        // u8g2->firstPage();
        u8g2->clearBuffer(); // Use clearBuffer for full buffer mode
        
        if (protocol->getSpeedKmh() > 0) {
            lastSpeedNotZero = millis();
        }
        
        if (millis() - lastSpeedNotZero > 5000) {
            u8g2->setContrast(0);
        } else {
            u8g2->setContrast(255);
        }
    } 

    // if (updatingDisplay) {
    uint16_t totalSeconds;
    uint8_t hours, minutes, seconds;

    switch (updateState)
    {
    case 1:
    
        //Display is SSD1309 in portrait mode 64x128
        //Speed
        u8g2->setFont(u8g2_font_logisoso54_tn);
        u8g2->setCursor(-3, 54);
        if (protocol->getSpeedKmh() > 10.0f) {
            u8g2->print((int)protocol->getSpeedKmh(), 1);
        } else {
            u8g2->print((int)protocol->getSpeedKmh(), 1);
            u8g2->setFont(u8g2_font_logisoso34_tn);
            u8g2->drawStr(29, 54, ".");
            u8g2->setCursor(40, 54);
            u8g2->print((int)((protocol->getSpeedKmh() - (int)protocol->getSpeedKmh()) * 10), 1);
        }

        //Separator lines
        u8g2->drawVLine(27, 55, 25);

        updateState++;
        break;
    case 2:
        
        //PAS Level
        u8g2->setFont(u8g2_font_logisoso20_tn);
        if (protocol->getAssistanceLevel() > 9) {
            u8g2->setCursor(-1, 78);
        } else {
            u8g2->setCursor(7, 78);
        }
        u8g2->print(protocol->getAssistanceLevel(), 1);

        //Separator lines
        u8g2->drawHLine(0, 55, 64);
            
        updateState++;
        break;
    case 3:

        //Temperature. If positive print with + sign
        u8g2->setFont(u8g2_font_logisoso16_tn);
        u8g2->setCursor(28, 75);
        if (protocol->getTemperature() > 0) {
            u8g2->print("+");
        } else if (protocol->getTemperature() < 0) {
            u8g2->print("-");
        }
        u8g2->setFont(u8g2_font_logisoso20_tn);
        u8g2->setCursor(37, 78);
        u8g2->print((int)protocol->getTemperature(), 1);

        updateState++;
        break;
    case 4:

        //Wattage. It is a 14S Li-ion battery so max 58.8V. Assume for now that the V is average 50V
        u8g2->setFont(u8g2_font_profont22_mn);
        if (protocol->unlimitedMode) {
            u8g2->drawStr(9, 95, u8x8_u16toa((int)(protocol->getCurrent() * 50), 4));
        } else {
            u8g2->drawStr(15, 95, u8x8_u16toa((int)(protocol->getCurrent() * 50), 3));
        }

        //Separator lines
        u8g2->drawHLine(0, 79, 64);
        u8g2->drawHLine(0, 96, 64);
        
        updateState++;
        break;
    case 5:

        //Average speed
        u8g2->setFont(u8g2_font_profont12_mr);
        averageSpeed = (uint16_t)((protocol->getTripMeter() / 1000.0f) / (protocol->getSessionTimeMs() / 3600000.0f));
        u8g2->drawStr(0, 106, u8x8_u16toa(averageSpeed / 100, 2));
        u8g2->drawStr(11, 106, ".");
        u8g2->drawStr(17, 106, u8x8_u16toa(averageSpeed % 100, 2));

        //Various statuses 1
        // for (int i = 7; i >= 0; i--) {
        //     u8g2->setCursor(42 - i * 6, 106);
        //     u8g2->print((protocol->getControllerStatus1() & (1 << i)) ? '1' : '0');
        // }

        u8g2->drawVLine(32, 96, 12);

        updateState++;
        break;
    case 6:

        //Cumulative Ah used
        u8g2->drawStr(36, 106, u8x8_u16toa(protocol->getCumulativeAh(), 2));
        u8g2->drawStr(47, 106, ".");
        u8g2->drawStr(53, 106, u8x8_u16toa(((int)(protocol->getCumulativeAh() * 100)) % 100, 2));

        u8g2->drawHLine(0, 107, 64);

        updateState++;
        break;
    case 7:

        //Various statuses 2
        // for (int i = 7; i >= 0; i--) {
        //     u8g2->setCursor(42 - i * 6, 117);
        //     u8g2->print((protocol->getControllerStatus2() & (1 << i)) ? '1' : '0');
        // }
        
        // Display time elapsed since boot as hh:mm:ss
        totalSeconds = protocol->getSessionTimeMs() / 1000;
        hours = totalSeconds / 3600;
        minutes = (totalSeconds % 3600) / 60;
        seconds = totalSeconds % 60;
        u8g2->drawStr(8, 117, u8x8_u16toa(hours, 2));
        u8g2->drawStr(20, 117, ":");
        u8g2->drawStr(27, 117, u8x8_u16toa(minutes, 2));
        u8g2->drawStr(39, 117, ":");
        u8g2->drawStr(46, 117, u8x8_u16toa(seconds, 2));

        //Separator lines
        u8g2->drawHLine(0, 118, 64);
        u8g2->drawVLine(32, 118, 10);

        updateState++;
        break;
    case 8:

        //Odometer
        u8g2->drawStr(0, 128, u8x8_u16toa(protocol->getOdometer(), 5));

        updateState++;
        break;
    case 9:

        //Trip meter
        u8g2->drawStr(36, 128, u8x8_u16toa(protocol->getTripMeter() / 100000, 3));
        u8g2->drawStr(53, 128, ".");
        u8g2->setCursor(59, 128);
        u8g2->print((int)(((protocol->getTripMeter() / 100000.0f) - (protocol->getTripMeter() / 100000)) * 10), 1);
        
        // if (!u8g2->nextPage()) {
        //     updatingDisplay = false; // Finished updating display
        // }
        updateState = 0;
        u8g2->sendBuffer(); // Use sendBuffer for full buffer mode
    default:
        break;
    }
}