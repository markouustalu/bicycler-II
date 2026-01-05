#include "Buttons.h"

Buttons* Buttons::instance = nullptr;

Buttons::Buttons(NO2Protocol* protocol) : protocol(protocol) {
    instance = this;
}

void Buttons::init() {
    buttonUp.begin(4);
    buttonM.begin(3);
    buttonDown.begin(2);

    buttonUp.setClickHandler(buttonUpClick);
    buttonUp.setTripleClickHandler(buttonUpTripleClick);
    buttonDown.setClickHandler(buttonDownClick);
    buttonM.setTripleClickHandler(buttonMTripleClick);
    buttonM.setDoubleClickHandler(buttonMDoubleClick);
    // buttonUp.setLongClickDetectedHandler(buttonUpLongClickDetected);
    // buttonUp.setLongClickHandler(buttonUpLongClickEnd);
}

void Buttons::loop() {
    buttonUp.loop();
    buttonM.loop();
    buttonDown.loop();
}

void Buttons::buttonUpClick(Button2& btn) {
    if (instance && instance->protocol) {
        instance->protocol->setAssistanceLevel(instance->protocol->getAssistanceLevel() + 1);
        instance->protocol->stateChanged = true;
    }
}

void Buttons::buttonUpTripleClick(Button2& btn) {
    if (instance && instance->protocol && millis() < 5000) { // Only allow triple click in first 5 seconds after power on
        instance->protocol->setUnlimitedMode();
    }
}

// void Buttons::buttonUpLongClickDetected(Button2& btn) {
//     if (instance && instance->protocol) {
//         instance->protocol->setCruise(true);
//         instance->protocol->stateChanged = true;
//     }
// }

// void Buttons::buttonUpLongClickEnd(Button2& btn) {
//     if (instance && instance->protocol) {
//         instance->protocol->setCruise(false);
//         instance->protocol->stateChanged = true;
//     }
// }

void Buttons::buttonDownClick(Button2& btn) {
    if (instance && instance->protocol) {
        instance->protocol->setAssistanceLevel(instance->protocol->getAssistanceLevel() - 1);
    }
}

void Buttons::buttonMTripleClick(Button2& btn) {
    if (instance && instance->protocol) {
        // Reset in-memory values
        instance->protocol->setCumulativeAh(0.0f);
        instance->protocol->setTripMeter(0);
        instance->protocol->setSessionTimeMs(0);
    }
}
void Buttons::buttonMDoubleClick(Button2& btn) {
    if (instance && instance->protocol) {
        // Trigger EEPROM save of current values
        instance->protocol->config->manualSave();
    }
}