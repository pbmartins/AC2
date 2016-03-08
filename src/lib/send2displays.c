#ifndef __SEND2DISPLAYS__
#define __SEND2DISPLAYS__

#include <detpic32.h>
#include "delay.c"

/**************************************/
/* Convert char to binary coded value */
/**************************************/
unsigned char toBcd(unsigned char value){
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
    static const unsigned char display7Scodes[] = {
        0b01110111,                                // 0
        0b01000001,                                // 1
        0b00111011,                                // 2
        0b01101011,                                // 3
        0b01001101,                                // 4
        0b01101110,                                // 5
        0b01111110,                                // 6
        0b01000011,                                // 7
        0b01111111,                                // 8
        0b01101111,                                // 9
        0b01011111,                                // a
        0b01111100,                                // b
        0b00110110,                                // c
        0b01111001,                                // d
        0b00111110,                                // e
        0b00011110,                                // f
        0b00000000                                 // null
    };
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
#endif /* __SEND2DISPLAYS__ */
