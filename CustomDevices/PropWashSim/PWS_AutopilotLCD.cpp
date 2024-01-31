#include "PWS_AutopilotLCD.h"

void
PWS_AutopilotLCD::begin(uint8_t IVA)
{
    Wire.setClock(400000);
	Wire.begin();
	Wire.beginTransmission(0x3F);
	// F: 0-80Hz, 1-160Hz
	// S: 1-osc on
	// E: 1-enable
	// M: 0-1/3 bias, 1-1/2 bias
	//...........100FSE0M
	Wire.write(0b10001100);
	Wire.endTransmission();
	Wire.beginTransmission(0x3F);
	Wire.write(0x70 | (IVA & 0x0F));
	Wire.endTransmission();
	Wire.beginTransmission(0x3F);
	Wire.write(0);
	for (int i = 0; i < 22; i++) {
		Wire.write(0);
	}
	Wire.endTransmission();
}

void
PWS_AutopilotLCD::lampTest(int ms)
{
	// we don't do anything in case we don't have a positive number
	if (ms<=0) return;

	Wire.beginTransmission(0x3F);
	Wire.write(0);
	for (int i = 0; i < 22; i++) {
		Wire.write(255);
	}
	Wire.endTransmission();
	delay(ms);
	clear();
}

void 
PWS_AutopilotLCD::clear() 
{
	Wire.beginTransmission(0x3F);
	Wire.write(0);
	for (int i = 0; i < 22; i++) {
		Wire.write(0);
	}
	Wire.endTransmission();
}

enum {DIGIT_BLANK=10, DIGIT_MINUS};
void
PWS_AutopilotLCD::digits(int32_t num)
{
	uint8_t			preserve[5];
	uint32_t		n;
	const uint8_t	segLut[12] = {0b11101011, 0b01100000, 0b11000111, 0b11100101, 0b01101100,
						0b10101101, 0b10101111, 0b11100000, 0b11101111, 0b11101100, 0b0, 0b00000100};
	uint8_t			digit[5];

	Wire.beginTransmission(0x3F);
	Wire.write(9);
	Wire.endTransmission();
	Wire.requestFrom(0x3F, 5);
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

	Wire.beginTransmission(0x3F);
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
	uint8_t		preserve[22];

	Wire.beginTransmission(0x3F);
	Wire.write(0);
	Wire.endTransmission();
	Wire.requestFrom(0x3F, 22);
	while (!Wire.available());
	for (int i = 0; i < 22; i++)
		preserve[i] = Wire.read();

	switch (display) {
	  case B1_CLR:
		Wire.beginTransmission(0x3F);
		Wire.write(0x00);
		Wire.write(0x00);
		Wire.write(0x00);
		Wire.write(0x00);
		Wire.write(0x00);
		Wire.write(0x00 | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B1_HDG:
		Wire.beginTransmission(0x3F);
		Wire.write(0x00);
		Wire.write(0x0E);
		Wire.write(0xAE);
		Wire.write(0x06);
		Wire.write(0x8B);
		Wire.write(0x06 | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B1_AP:
		Wire.beginTransmission(0x3F);
		Wire.write(0x00);
		Wire.write(0x8E);
		Wire.write(0x0E | 0xE0);
		Wire.write(0x88);
		Wire.write(0x00);
		Wire.write(0x00 | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B1_NAV:
		Wire.beginTransmission(0x3F);
		Wire.write(0x00);
		Wire.write(0x6A);
		Wire.write(0x0A | 0xE0);
		Wire.write(0xC8);
		Wire.write(0x60);
		Wire.write(0x0A | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B1_ROL:
		Wire.beginTransmission(0x3F);
		Wire.write(0x00);
		Wire.write(0xAE);
		Wire.write(0x0C | 0xB0);
		Wire.write(0xC8);
		Wire.write(0x0B);
		Wire.write(0x00 | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B1_APR:
		Wire.beginTransmission(0x3F);
		Wire.write(0x00);
		Wire.write(0x8E);
		Wire.write(0x0E | 0xE0);
		Wire.write(0x88);
		Wire.write(0xAE);
		Wire.write(0x0C | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B1_REV:
		Wire.beginTransmission(0x3F);
		Wire.write(0x00);
		Wire.write(0xAE);
		Wire.write(0x0C | 0xF0);
		Wire.write(0x08);
		Wire.write(0x60);
		Wire.write(0x0A | (preserve[4] & 0b11110001));
		Wire.endTransmission();
		break;
	  case B2_CLR:
		Wire.beginTransmission(0x3F);
		Wire.write(5);
		Wire.write(0x00 | (preserve[5] & 0b00010000));
		Wire.write(0x00 | (preserve[6] & 0b00010000));
		Wire.write(0x00 | (preserve[7] & 0b00000001));
		Wire.write(0x00 | (preserve[8] & 0b00010001));
		Wire.endTransmission();
		break;
	  case B2_ALT:
		Wire.beginTransmission(0x3F);
		Wire.write(5);
		Wire.write(0xEE | (preserve[5] & 0b00010000));
		Wire.write(0x0B | (preserve[6] & 0b00010000));
		Wire.write(0x00 | (preserve[7] & 0b00000001));
		Wire.write(0x0E | (preserve[8] & 0b00010001));
		Wire.endTransmission();
		break;
	  case B2_PFT:
		Wire.beginTransmission(0x3F);
		Wire.write(5);
		Wire.write(0xCE | (preserve[5] & 0b00010000));
		Wire.write(0x8E | (preserve[6] & 0b00010000));
		Wire.write(0x04 | (preserve[7] & 0b00000001));
		Wire.write(0x0E | (preserve[8] & 0b00010001));
		Wire.endTransmission();
		break;
	  case B2_VS:
		Wire.beginTransmission(0x3F);
		Wire.write(5);
		Wire.write(0x00 | (preserve[5] & 0b00010000));
		Wire.write(0x60 | (preserve[6] & 0b00010000));
		Wire.write(0xDA | (preserve[7] & 0b00000001));
		Wire.write(0x48 | (preserve[8] & 0b00010001));
		Wire.endTransmission();
		break;
	  case B2_GS:
		Wire.beginTransmission(0x3F);
		Wire.write(5);
		Wire.write(0x00 | (preserve[5] & 0b00010000));
		Wire.write(0x8B | (preserve[6] & 0b00010000));
		Wire.write(0xD6 | (preserve[7] & 0b00000001));
		Wire.write(0x48 | (preserve[8] & 0b00010001));
		Wire.endTransmission();
		break;
	  case B3_CLR:
		Wire.beginTransmission(0x3F);
		Wire.write(18);
		Wire.write(preserve[18] & 1);
		Wire.write(0x00);
		Wire.write(0x00);
		Wire.write(0x00);
		Wire.endTransmission();
		break;
	  // Block 3 and 4 digits are in reverse order
	  case B3_REV:
		Wire.beginTransmission(0x3F);
		Wire.write(18);
		Wire.write(0x6A | (preserve[18] & 1));	//D14
		Wire.write(0xA0 | 0x00);				//D13 | D14
		Wire.write(0xC0 | 0x0F);				//D12 | D13
		Wire.write(0xEA);						//D12
		Wire.endTransmission();
		break;
	  case B3_NAV:
		Wire.beginTransmission(0x3F);
		Wire.write(18);
		Wire.write(0x6A | (preserve[18] & 1));	//D14
		Wire.write(0xF0 | 0x00);				//D13 | D14
		Wire.write(0xA0 | 0x0E);				//D12 | D13
		Wire.write(0xA6);						//D12
		Wire.endTransmission();
		break;
	  case B3_APR:
		Wire.beginTransmission(0x3F);
		Wire.write(18);
		Wire.write(0xAC | (preserve[18] & 1));	//D14
		Wire.write(0xE0 | 0x0E);				//D13 | D14
		Wire.write(0xE0 | 0x0E);				//D12 | D13
		Wire.write(0xE8);						//D12
		Wire.endTransmission();
		break;
	  case B3_GS:
		Wire.beginTransmission(0x3F);
		Wire.write(18);
		Wire.write(0x86 | (preserve[18] & 1));	//D14
		Wire.write(0xB0 | 0x0D);				//D13 | D14
		Wire.write(0x00 | 0x0B);				//D12 | D13
		Wire.write(0x00);						//D12
		Wire.endTransmission();
		break;
	  case B4_CLR:
		Wire.beginTransmission(0x3F);
		Wire.write(14);
		Wire.write(0x00);						//D17
		Wire.write(0x00 | 0x00);				//D16 | D17
		Wire.write(0x00 | 0x00);				//D15 | D16
		Wire.write(0x00);						//D15
		Wire.endTransmission();
		break;
	  case B4_ALT:
		Wire.beginTransmission(0x3F);
		Wire.write(14);
		Wire.write(0xE0);						//L14 D17
		Wire.write(0x00 | 0x00);				//L15 D16 | D17
		Wire.write(0xC0 | 0x0B);				//L16 D15 | D16
		Wire.write(0xE8);						//L17 D15
		Wire.endTransmission();
		break;
	  case B4_GS:
		Wire.beginTransmission(0x3F);
		Wire.write(14);
		Wire.write(0x84);						//L14 D17
		Wire.write(0xB0 | 0x0D);				//L15 D16 | D17
		Wire.write(0x00 | 0x0B);				//L16 D15 | D16
		Wire.write(0x00);						//L17 D15
		Wire.endTransmission();
		break;
	}
}
		
void
PWS_AutopilotLCD::annunciator(enum ANNUNCIATOR display, bool state)
{
	uint8_t		preserve[22];

	Wire.beginTransmission(0x3F);
	Wire.write(0);
	Wire.endTransmission();
	Wire.requestFrom(0x3F, 22);
	while (!Wire.available());
	for (int i = 0; i < 22; i++)
		preserve[i] = Wire.read();

	switch (display) {
	  case AN_AP:
		Wire.beginTransmission(0x3F);
		Wire.write(5);
		Wire.write((preserve[5] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_YD:
		Wire.beginTransmission(0x3F);
		Wire.write(4);
		Wire.write((preserve[4] & ~1) | (state ? 1 : 0));
		Wire.endTransmission();
		break;
	  case AN_L_ARM:
		Wire.beginTransmission(0x3F);
		Wire.write(18);
		Wire.write((preserve[18] & ~1) | (state ? 1 : 0));
		Wire.endTransmission();
		break;
	  case AN_R_ARM:
		Wire.beginTransmission(0x3F);
		Wire.write(4);
		Wire.write((preserve[4] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_DOWN:
		Wire.beginTransmission(0x3F);
		Wire.write(6);
		Wire.write((preserve[6] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_PT:
		Wire.beginTransmission(0x3F);
		Wire.write(7);
		Wire.write((preserve[7] & ~1) | (state ? 1 : 0));
		Wire.write((preserve[8] & ~1) | (state ? 1 : 0));
		Wire.endTransmission();
		break;
	  case AN_UP:
		Wire.beginTransmission(0x3F);
		Wire.write(8);
		Wire.write((preserve[8] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_ALERT:
		Wire.beginTransmission(0x3F);
		Wire.write(9);
		Wire.write((preserve[9] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_FPM:
		Wire.beginTransmission(0x3F);
		Wire.write(11);
		Wire.write((preserve[11] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_FT:
		Wire.beginTransmission(0x3F);
		Wire.write(13);
		Wire.write((preserve[13] & ~(1<<4)) | (state ? 1<<4 : 0));
		Wire.endTransmission();
		break;
	  case AN_HPA:
		Wire.beginTransmission(0x3F);
		Wire.write(4);
		Wire.write((preserve[4] & ~(1<<5)) | (state ? 1<<5 : 0));
		Wire.endTransmission();
		break;
	  case AN_INHG:
		Wire.beginTransmission(0x3F);
		Wire.write(4);
		Wire.write((preserve[4] & ~(1<<6 | 1<<7)) | (state ? 1<<6 | 1<<7: 0));
		Wire.endTransmission();
		break;
	}
}