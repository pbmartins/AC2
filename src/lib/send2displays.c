#include "displays.h"
#include <detpic32.h>
#include "delay.h"

/**************************************/
/* Convert char to binary coded value */
/**************************************/
unsigned char toBcd(unsigned char value) {
   return ((value / 10) << 4) + (value % 10);
}

/**************************************/
/* Turn on the dots on 7seg displays  */
/* Accepts a char that indicates which point is on */
/* 0x0011 -> both on                  */
/* 0x0010 -> just high on             */
/* 0x0001 -> just low on              */
/**************************************/
void send2dots(unsigned char value) {
    static unsigned char displayFlagDots = 0;

    // Configure B ports as outputs
    TRISB = (TRISB & 0xFFC00);

    unsigned int digit_low, digit_high;
    digit_high = (value & 0x00F0) >> 4;
    digit_low = value & 0x0F;

    if (!displayFlagDots) {
        // select display low
        LATBbits.LATB9 = 1;
        LATBbits.LATB8 = 0;
        if (digit_low)
            LATBbits.LATB7 = 1;
        else
            LATBbits.LATB7 = 0;
    } else {
        // select display high
        LATBbits.LATB9 = 0;
        LATBbits.LATB8 = 1;
        if (digit_high)
            LATBbits.LATB7 = 1;
        else
            LATBbits.LATB7 = 0;
    }
    displayFlagDots = !displayFlagDots;
}

/**************************************/
/* Displays a char on 7 seg display   */
/**************************************/
void send2displays(unsigned char value) {
    static unsigned char displayFlag = 0;
    unsigned int digit_low, digit_high;

    if (value != 60) {
        digit_high = (value & 0x00F0) >> 4;
        digit_low = value & 0x0F;
    } else
        digit_high = digit_low = 16;

    // Configure B ports as outputs
    TRISB = (TRISB & 0xFFC00);

    if (!displayFlag) {
        // select display low
        LATBbits.LATB9 = 1;
        LATBbits.LATB8 = 0;
        LATB = (LATB & 0xFF00) | display7Scodes[digit_low];
    } else {
        // select display high
        LATBbits.LATB9 = 0;
        LATBbits.LATB8 = 1;
        LATB = (LATB & 0xFF00) | display7Scodes[digit_high];
    }
    displayFlag = !displayFlag;
}
