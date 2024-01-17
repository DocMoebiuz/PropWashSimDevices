#include "PWS_AutopilotLCD.h"

void
PWS_AutopilotLCD::begin(uint8_t i2cAdress, uint8_t IVA)
{
	_i2cAdress = i2cAdress;
    Wire.setClock(400000);
	Wire.begin();
	Wire.beginTransmission(_i2cAdress);
	// F: 0-80Hz, 1-160Hz
	// S: 1-osc on
	// E: 1-enable
	// M: 0-1/3 bias, 1-1/2 bias
	//...........100FSE0M
	Wire.write(0b10001100);
	Wire.endTransmission();
	Wire.beginTransmission(_i2cAdress);
	Wire.write(0x70 | (IVA & 0x0F));
	Wire.endTransmission();
	Wire.beginTransmission(_i2cAdress);
	Wire.write(0);
	for (int i = 0; i < 22; i++) {
		Wire.write(0);
	}
	Wire.endTransmission();
	_initialized = true;
}

void
PWS_AutopilotLCD::lampTest(bool start_lamptest)
{
	if (!_initialized)
		return;
	// we don't do anything in case we don't have a positive number
	if (_duration_lamptest <= 0) return;

	if (start_lamptest) {
		Wire.beginTransmission(_i2cAdress);
		Wire.write(0);
		for (int i = 0; i < 22; i++) {
			Wire.write(255);
		}
		Wire.endTransmission();
		_start_lamptest_ms = millis();
		_lamptest_started = true;
	}
	// just to avoid the "long" calculation of the next if() clause
	if (!_lamptest_started)
		return;

	if (millis() > _start_lamptest_ms + _duration_lamptest ) {
		Wire.beginTransmission(_i2cAdress);
		Wire.write(0);
		for (int i = 0; i < 22; i++) {
			Wire.write(0);
		}
		Wire.endTransmission();
		_lamptest_started = false;
	}	
}

enum {DIGIT_BLANK=10, DIGIT_MINUS};
void
PWS_AutopilotLCD::digits(int32_t num)
{
	if (!_initialized)
		return;

	uint8_t			preserve[5];
	uint32_t		n;
	const uint8_t	segLut[12] = {0b11101011, 0b01100000, 0b11000111, 0b11100101, 0b01101100,
						0b10101101, 0b10101111, 0b11100000, 0b11101111, 0b11101100, 0b0, 0b00000100};
	uint8_t			digit[5];

	Wire.beginTransmission(_i2cAdress);
	Wire.write(9);
	Wire.endTransmission();
	Wire.requestFrom((int)_i2cAdress, 5);
	while (!Wire.available());
	for (int i = 0; i < 5; i++)
		preserve[i] = Wire.read();

	if (num > 0)
		n = num % 100000;
	else
		n = abs(num) % 10000;

	for (int i = 4; i >= 0; i--) {
		digit[i] = n % 10;
		n /= 10;
	}
	for (int i = 0; i < 4 && !digit[i]; i++)
		if (!digit[i] && digit[i+1] && (num < 0))
			digit[i] = DIGIT_MINUS;
		else if (!digit[i])
			digit[i] = DIGIT_BLANK;

	Wire.beginTransmission(_i2cAdress);
	Wire.write(0x09);
	Wire.write(segLut[digit[0]] | (preserve[0] & 0b00010000));
	Wire.write(segLut[digit[1]] | (num >= 1000 || num <= -1000 ? 0b00010000 : 0));
	Wire.write(segLut[digit[2]] | (preserve[2] & 0b00010000));
	Wire.write(segLut[digit[3]]);
	Wire.write(segLut[digit[4]] | (preserve[4] & 0b00010000));
	Wire.endTransmission();
}

void
PWS_AutopilotLCD::prompt(enum PROMPT display)
{
	if (!_initialized)
		return;

	uint8_t		preserve[22];

	Wire.beginTransmission(_i2cAdress);
	Wire.write(0);
	Wire.endTransmission();
	Wire.requestFrom((int)_i2cAdress, 22);
	while (!Wire.available());
	for (int i = 0; i < 22; i++)
		preserve[i] = Wire.read();

	switch (display) {
	  case B1_CLR:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(0x00);
		Wire.write(0x00);
		Wire.write(0x00);
		Wire.write(0x00);
		Wire.write(0x00);
		Wire.write(0x00 | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B1_HDG:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(0x00);
		Wire.write(0x0E);
		Wire.write(0xAE);
		Wire.write(0x06);
		Wire.write(0x8B);
		Wire.write(0x06 | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B1_AP:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(0x00);
		Wire.write(0x8E);
		Wire.write(0x0E | 0xE0);
		Wire.write(0x88);
		Wire.write(0x00);
		Wire.write(0x00 | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B1_NAV:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(0x00);
		Wire.write(0x6A);
		Wire.write(0x0A | 0xE0);
		Wire.write(0xC8);
		Wire.write(0x60);
		Wire.write(0x0A | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B1_ROL:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(0x00);
		Wire.write(0xAE);
		Wire.write(0x0C | 0xB0);
		Wire.write(0xC8);
		Wire.write(0x0B);
		Wire.write(0x00 | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B1_APR:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(0x00);
		Wire.write(0x8E);
		Wire.write(0x0E | 0xE0);
		Wire.write(0x88);
		Wire.write(0xAE);
		Wire.write(0x0C | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B1_REV:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(0x00);
		Wire.write(0xAE);
		Wire.write(0x0C | 0xF0);
		Wire.write(0x08);
		Wire.write(0x60);
		Wire.write(0x0A | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B2_CLR:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(5);
		Wire.write(0x00 | (preserve[5] & 0b00010000));
		Wire.write(0x00 | (preserve[6] & 0b00010000));
		Wire.write(0x00 | (preserve[7] & 0b00000001));
		Wire.write(0x00 | (preserve[8] & 0b00010001));
		Wire.endTransmission();
		break;
	  case B2_ALT:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(5);
		Wire.write(0xEE | (preserve[5] & 0b00010000));
		Wire.write(0x0B | (preserve[6] & 0b00010000));
		Wire.write(0x00 | (preserve[7] & 0b00000001));
		Wire.write(0x0E | (preserve[8] & 0b00010001));
		Wire.endTransmission();
		break;
	  case B2_PFT:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(5);
		Wire.write(0xCE | (preserve[5] & 0b00010000));
		Wire.write(0x8E | (preserve[6] & 0b00010000));
		Wire.write(0x04 | (preserve[7] & 0b00000001));
		Wire.write(0x0E | (preserve[8] & 0b00010001));
		Wire.endTransmission();
		break;
	  case B2_VS:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(5);
		Wire.write(0x00 | (preserve[5] & 0b00010000));
		Wire.write(0x60 | (preserve[6] & 0b00010000));
		Wire.write(0xDA | (preserve[7] & 0b00000001));
		Wire.write(0x48 | (preserve[8] & 0b00010001));
		Wire.endTransmission();
		break;
	  case B2_GS:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(5);
		Wire.write(0x00 | (preserve[5] & 0b00010000));
		Wire.write(0x8B | (preserve[6] & 0b00010000));
		Wire.write(0xD6 | (preserve[7] & 0b00000001));
		Wire.write(0x48 | (preserve[8] & 0b00010001));
		Wire.endTransmission();
		break;
	  case B3_CLR:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(18);
		Wire.write(preserve[18] & 1);
		Wire.write(0x00);
		Wire.write(0x00);
		Wire.write(0x00);
		Wire.endTransmission();
		break;
	  // Block 3 and 4 digits are in reverse order
	  case B3_REV:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(18);
		Wire.write(0x6A | (preserve[18] & 1));	//D14
		Wire.write(0xA0 | 0x00);				//D13 | D14
		Wire.write(0xC0 | 0x0F);				//D12 | D13
		Wire.write(0xEA);						//D12
		Wire.endTransmission();
		break;
	  case B3_NAV:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(18);
		Wire.write(0x6A | (preserve[18] & 1));	//D14
		Wire.write(0xF0 | 0x00);				//D13 | D14
		Wire.write(0xA0 | 0x0E);				//D12 | D13
		Wire.write(0xA6);						//D12
		Wire.endTransmission();
		break;
	  case B3_APR:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(18);
		Wire.write(0xAC | (preserve[18] & 1));	//D14
		Wire.write(0xE0 | 0x0E);				//D13 | D14
		Wire.write(0xE0 | 0x0E);				//D12 | D13
		Wire.write(0xE8);						//D12
		Wire.endTransmission();
		break;
	  case B3_GS:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(18);
		Wire.write(0x86 | (preserve[18] & 1));	//D14
		Wire.write(0xB0 | 0x0D);				//D13 | D14
		Wire.write(0x00 | 0x0B);				//D12 | D13
		Wire.write(0x00);						//D12
		Wire.endTransmission();
		break;
	  case B4_CLR:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(14);
		Wire.write(0x00);						//D17
		Wire.write(0x00 | 0x00);				//D16 | D17
		Wire.write(0x00 | 0x00);				//D15 | D16
		Wire.write(0x00);						//D15
		Wire.endTransmission();
		break;
	  case B4_ALT:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(14);
		Wire.write(0xE0);						//D17
		Wire.write(0x00 | 0x00);				//D16 | D17
		Wire.write(0xE0 | 0x0B);				//D15 | D16
		Wire.write(0xE8);						//D15
		Wire.endTransmission();
		break;
	}
}
		
void
PWS_AutopilotLCD::annunciator(enum ANNUNCIATOR display, bool state)
{
	if (!_initialized)
		return;

	uint8_t		preserve[22];

	Wire.beginTransmission(_i2cAdress);
	Wire.write(0);
	Wire.endTransmission();
	Wire.requestFrom((int)_i2cAdress, 22);
	while (!Wire.available());
	for (int i = 0; i < 22; i++)
		preserve[i] = Wire.read();

	switch (display) {
	  case AN_AP:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(5);
		Wire.write((preserve[5] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_YD:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(4);
		Wire.write((preserve[4] & ~1) | (state ? 1 : 0));
		Wire.endTransmission();
		break;
	  case AN_L_ARM:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(18);
		Wire.write((preserve[18] & ~1) | (state ? 1 : 0));
		Wire.endTransmission();
		break;
	  case AN_R_ARM:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(4);
		Wire.write((preserve[4] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_DOWN:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(6);
		Wire.write((preserve[6] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_PT:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(7);
		Wire.write((preserve[7] & ~1) | (state ? 1 : 0));
		Wire.write((preserve[8] & ~1) | (state ? 1 : 0));
		Wire.endTransmission();
		break;
	  case AN_UP:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(8);
		Wire.write((preserve[8] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_ALERT:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(9);
		Wire.write((preserve[9] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_FPM:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(11);
		Wire.write((preserve[11] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_FT:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(13);
		Wire.write((preserve[13] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_HPA:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(4);
		Wire.write((preserve[4] & ~(1<<5)) | (state ? 1<<5 : 0));
		Wire.endTransmission();
		break;
	  case AN_INHG:
		Wire.beginTransmission(_i2cAdress);
		Wire.write(4);
		Wire.write((preserve[4] & ~(1<<6 | 1<<7)) | (state ? 1<<6 | 1<<7: 0));
		Wire.endTransmission();
		break;
	}
}

void PWS_AutopilotLCD::set(int16_t messageID, char *setPoint)
{
	if (!_initialized)
		return;

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
        prompt(B1_CLR);
        prompt(B2_CLR);
        prompt(B3_CLR);
        prompt(B4_CLR);
        annunciator(AN_AP, false);
        annunciator(AN_YD, false);
        annunciator(AN_L_ARM, false);
        annunciator(AN_R_ARM, false);
        annunciator(AN_DOWN, false);
        annunciator(AN_PT, false);
        annunciator(AN_UP, false);
        annunciator(AN_ALERT, false);
        annunciator(AN_FPM, false);
        annunciator(AN_FT, false);
        annunciator(AN_HPA, false);
        annunciator(AN_INHG, false);
        break;

    case -2:
        // tbd., get's called when PowerSavingMode is entered'
        break;
    // SetDigit
    case 0:
        digits(numberData);
        break;
    // LampTest
    case 1:
		_duration_lamptest = numberData;
        lampTest(true);
        break;
    // Set B1 prompt
    case 2:
        if (strcmp(setPoint, "HDG") == 0) prompt(B1_HDG); else
        if (strcmp(setPoint, "AP") == 0)  prompt(B1_AP); else
        if (strcmp(setPoint, "NAV") == 0) prompt(B1_NAV); else
        if (strcmp(setPoint, "ROL") == 0) prompt(B1_ROL); else
        if (strcmp(setPoint, "APR") == 0) prompt(B1_APR); else
        if (strcmp(setPoint, "REV") == 0) prompt(B1_REV); else
        if (strcmp(setPoint, "CLR") == 0) prompt(B1_CLR); else
        if (numberData==0) prompt(B1_CLR);
        break;
    // Set B2 prompt
    case 3:
        if (strcmp(setPoint, "ALT") == 0) prompt(B2_ALT); else
        if (strcmp(setPoint, "PFT") == 0)  prompt(B2_PFT); else
        if (strcmp(setPoint, "VS") == 0) prompt(B2_VS); else
        if (strcmp(setPoint, "GS") == 0) prompt(B2_GS); else
        if (strcmp(setPoint, "CLR") == 0) prompt(B2_CLR); else
        if (numberData==0) prompt(B2_CLR);
        break;
    // Set B3 prompt
    case 4:
        if (strcmp(setPoint, "REV") == 0) prompt(B3_REV); else
        if (strcmp(setPoint, "NAV") == 0)  prompt(B3_NAV); else
        if (strcmp(setPoint, "APR") == 0) prompt(B3_APR); else
        if (strcmp(setPoint, "GS") == 0) prompt(B3_GS); else
        if (strcmp(setPoint, "CLR") == 0) prompt(B3_CLR); else
        if (numberData==0) prompt(B3_CLR);
        break;
    // Set B4 prompt
    case 5:        
        if (strcmp(setPoint, "ALT") == 0) prompt(B4_ALT); else
        if (strcmp(setPoint, "CLR") == 0) prompt(B4_CLR); else
        if (numberData==0) prompt(B4_CLR);
        break;

    // Turn ON annunciator AP
    case 6:
        annunciator(AN_AP, numberData!=0);
        break;

    // Turn ON annunciator AN_YD
    case 7:
        annunciator(AN_YD, numberData!=0);
        break;

    // Turn ON annunciator AN_L_ARM
    case 8:
        annunciator(AN_L_ARM, numberData!=0);
        break;

    // Turn ON annunciator AN_R_ARM
    case 9:
        annunciator(AN_R_ARM, numberData!=0);
        break;
    
    // Turn ON annunciator AN_DOWN
    case 10:
        annunciator(AN_DOWN, numberData!=0);
        break;

    // Turn ON annunciator AN_PT
    case 11:
        annunciator(AN_PT, numberData!=0);
        break;
    
    // Turn OFF annunciator AN_UP
    case 12:
        annunciator(AN_UP, numberData!=0);
        break;

    // Turn OFF annunciator AN_ALERT
    case 13:
        annunciator(AN_ALERT, numberData!=0);
        break;

    // Turn OFF annunciator AN_FPM
    case 14:
        annunciator(AN_FPM, numberData!=0);
        break;

    // Turn OFF annunciator AN_FT
    case 15:
        annunciator(AN_FT, numberData!=0);
        break;

    // Turn OFF annunciator AN_HPA
    case 16:
        annunciator(AN_HPA, numberData!=0);
        break;

    // Turn OFF annunciator AN_INHG
    case 17:
        annunciator(AN_INHG, numberData!=0);
        break;

    default:
        break;
    }
}

void PWS_AutopilotLCD::detach()
{
	_initialized = false;
}