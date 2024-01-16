

#if !defined(PWS_Autopilot_h)
#define PWS_Autopilot_h

#include "Arduino.h"
#include "Wire.h"

enum PROMPT {B1_HDG, B1_AP, B1_NAV, B1_ROL, B1_APR, B1_REV, B1_CLR,
            B2_ALT, B2_PFT, B2_VS, B2_GS, B2_CLR,
            B3_REV, B3_NAV, B3_APR, B3_GS, B3_CLR,
            B4_ALT, B4_CLR};
enum ANNUNCIATOR {AN_AP, AN_YD, AN_L_ARM, AN_R_ARM, AN_DOWN, AN_PT, AN_UP, AN_ALERT, AN_FPM, AN_FT, AN_HPA, AN_INHG};

class PWS_AutopilotLCD {
    public:
        /* Internal Voltage Adjust
         *
         * The LCD needs to be driven at 3.3V.  If the supply voltage is 5V, the IVA
         * needs to be set to 3.3/5 or 0.66 so 0xB or 0xC would be safe to use.
         * 
         * IVA  LCD drive voltage
         * 0x0 1.000 × VDD  (Default)
         * 0x1 0.957 × VDD
         * 0x2 0.918 × VDD
         * 0x3 0.882 × VDD
         * 0x4 0.849 × VDD
         * 0x5 0.818 × VDD
         * 0x6 0.789 × VDD
         * 0x7 0.763 × VDD
         * 0x8 0.738 × VDD
         * 0x9 0.714 × VDD
         * 0xA 0.692 × VDD
         * 0xB 0.672 × VDD
         * 0xC 0.652 × VDD
         * 0xD 0.634 × VDD
         * 0xE 0.616 × VDD
         * 0xF 0.600 × VDD
         */
        void begin(uint8_t IVA=0);
        void prompt(enum PROMPT display);
        void annunciator(enum ANNUNCIATOR, bool);
        void digits(int32_t num);
        void lampTest(int ms);
};
#endif