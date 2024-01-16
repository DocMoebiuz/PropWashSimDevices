#pragma once

#include "Arduino.h"
#include "PWS_AutopilotLCD.h"

class MyCustomClass
{
public:
    MyCustomClass();
    void begin();
    void attach();
    void detach();
    void set(int16_t messageID, char *setPoint);
    void update();

private:
    bool    _initialised;
    PWS_AutopilotLCD * _lcd;
};