#ifndef BUTTONS_H
#define BUTTONS_H

#include "Button2.h"
#include "NO2Protocol.h"

class Buttons {
public:
    Buttons(NO2Protocol* protocol);

    void init();
    void loop();

private:
    Button2 buttonUp;
    Button2 buttonM;
    Button2 buttonDown;
    NO2Protocol* protocol;

    static void buttonUpClick(Button2& btn);
    static void buttonUpTripleClick(Button2& btn);
    // static void buttonUpLongClickDetected(Button2& btn);
    // static void buttonUpLongClickEnd(Button2& btn);
    static void buttonDownClick(Button2& btn);
    static void buttonMTripleClick(Button2& btn);
    static void buttonMDoubleClick(Button2& btn);
    static Buttons* instance;
};

#endif // BUTTONS_H