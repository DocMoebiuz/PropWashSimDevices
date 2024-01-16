#include "MyCustomClass.h"
#include "allocateMem.h"
#include "commandmessenger.h"

/* **********************************************************************************
    This is just the basic code to set up your custom device.
    Change/add your code as needed.
********************************************************************************** */

MyCustomClass::MyCustomClass()
{
}

void MyCustomClass::begin()
{
    _lcd->begin(0xB);
}

void MyCustomClass::attach()
{
    /* **********************************************************************************
        Next call the constructor of your custom device
        adapt it to the needs of your constructor
    ********************************************************************************** */
    if (!FitInMemory(sizeof(PWS_AutopilotLCD))) {
        // Error Message to Connector
        cmdMessenger.sendCmd(kStatus, F("Custom Device does not fit in Memory"));
        return;
    }

    //_oledDisplay = new (allocateMemory(sizeof(U8G2_SSD1322_NHD_256X64_F_4W_SW_SPI))) U8G2_SSD1322_NHD_256X64_F_4W_SW_SPI(U8G2_R0, _clk, _data, _cs, _dc);
    _lcd = new (allocateMemory(sizeof(PWS_AutopilotLCD))) PWS_AutopilotLCD();
    begin();
}

void MyCustomClass::detach()
{
    if (!_initialised)
        return;
    _initialised = false;
}

void MyCustomClass::set(int16_t messageID, char *setPoint)
{
    /* **********************************************************************************
        Each messageID has it's own value
        check for the messageID and define what to do.
        Important Remark!
        MessageID == -1 will be send from the connector when Mobiflight is closed
        Put in your code to shut down your custom device (e.g. clear a display)
        MessageID == -2 will be send from the connector when PowerSavingMode is entered
        Put in your code to enter this mode (e.g. clear a display)

    ********************************************************************************** */
    int32_t numberData = atoi(setPoint);

    // do something according your messageID
    switch (messageID) {
    case -1:
        _lcd->prompt(B1_CLR);
        _lcd->prompt(B2_CLR);
        _lcd->prompt(B3_CLR);
        _lcd->prompt(B4_CLR);
        _lcd->annunciator(AN_AP, false);
        _lcd->annunciator(AN_YD, false);
        _lcd->annunciator(AN_L_ARM, false);
        _lcd->annunciator(AN_R_ARM, false);
        _lcd->annunciator(AN_DOWN, false);
        _lcd->annunciator(AN_PT, false);
        _lcd->annunciator(AN_UP, false);
        _lcd->annunciator(AN_ALERT, false);
        _lcd->annunciator(AN_FPM, false);
        _lcd->annunciator(AN_FT, false);
        _lcd->annunciator(AN_HPA, false);
        _lcd->annunciator(AN_INHG, false);
        break;

    case -2:
        // tbd., get's called when PowerSavingMode is entered'
        break;
    // SetDigit
    case 0:
        _lcd->digits(numberData);
        break;
    // LampTest
    case 1:
        _lcd->lampTest(numberData);
        break;
    // Set B1 prompt
    case 2:
        if (strcmp(setPoint, "HDG") == 0) _lcd->prompt(B1_HDG); else
        if (strcmp(setPoint, "AP") == 0)  _lcd->prompt(B1_AP); else
        if (strcmp(setPoint, "NAV") == 0) _lcd->prompt(B1_NAV); else
        if (strcmp(setPoint, "ROL") == 0) _lcd->prompt(B1_ROL); else
        if (strcmp(setPoint, "APR") == 0) _lcd->prompt(B1_APR); else
        if (strcmp(setPoint, "REV") == 0) _lcd->prompt(B1_REV); else
        if (strcmp(setPoint, "CLR") == 0) _lcd->prompt(B1_CLR); else
        if (numberData==0) _lcd->prompt(B1_CLR);
        break;
    // Set B2 prompt
    case 3:
        if (strcmp(setPoint, "ALT") == 0) _lcd->prompt(B2_ALT); else
        if (strcmp(setPoint, "PFT") == 0)  _lcd->prompt(B2_PFT); else
        if (strcmp(setPoint, "VS") == 0) _lcd->prompt(B2_VS); else
        if (strcmp(setPoint, "GS") == 0) _lcd->prompt(B2_GS); else
        if (strcmp(setPoint, "CLR") == 0) _lcd->prompt(B2_CLR); else
        if (numberData==0) _lcd->prompt(B2_CLR);
        break;
    // Set B3 prompt
    case 4:
        if (strcmp(setPoint, "REV") == 0) _lcd->prompt(B3_REV); else
        if (strcmp(setPoint, "NAV") == 0)  _lcd->prompt(B3_NAV); else
        if (strcmp(setPoint, "APR") == 0) _lcd->prompt(B3_APR); else
        if (strcmp(setPoint, "GS") == 0) _lcd->prompt(B3_GS); else
        if (strcmp(setPoint, "CLR") == 0) _lcd->prompt(B3_CLR); else
        if (numberData==0) _lcd->prompt(B3_CLR);
        break;
    // Set B4 prompt
    case 5:        
        if (strcmp(setPoint, "ALT") == 0) _lcd->prompt(B4_ALT); else
        if (strcmp(setPoint, "CLR") == 0) _lcd->prompt(B4_CLR); else
        if (numberData==0) _lcd->prompt(B4_CLR);
        break;

    // Turn ON annunciator AP
    case 6:
        _lcd->annunciator(AN_AP, numberData!=0);
        break;

    // Turn ON annunciator AN_YD
    case 7:
        _lcd->annunciator(AN_YD, numberData!=0);
        break;

    // Turn ON annunciator AN_L_ARM
    case 8:
        _lcd->annunciator(AN_L_ARM, numberData!=0);
        break;

    // Turn ON annunciator AN_R_ARM
    case 9:
        _lcd->annunciator(AN_R_ARM, numberData!=0);
        break;
    
    // Turn ON annunciator AN_DOWN
    case 10:
        _lcd->annunciator(AN_DOWN, numberData!=0);
        break;

    // Turn ON annunciator AN_PT
    case 11:
        _lcd->annunciator(AN_PT, numberData!=0);
        break;
    
    // Turn OFF annunciator AN_UP
    case 12:
        _lcd->annunciator(AN_UP, numberData!=0);
        break;

    // Turn OFF annunciator AN_ALERT
    case 13:
        _lcd->annunciator(AN_ALERT, numberData!=0);
        break;

    // Turn OFF annunciator AN_FPM
    case 14:
        _lcd->annunciator(AN_FPM, numberData!=0);
        break;

    // Turn OFF annunciator AN_FT
    case 15:
        _lcd->annunciator(AN_FT, numberData!=0);
        break;

    // Turn OFF annunciator AN_HPA
    case 16:
        _lcd->annunciator(AN_HPA, numberData!=0);
        break;

    // Turn OFF annunciator AN_INHG
    case 17:
        _lcd->annunciator(AN_INHG, numberData!=0);
        break;

    default:
        break;
    }
}

void MyCustomClass::update()
{
    // Do something which is required regulary
}
