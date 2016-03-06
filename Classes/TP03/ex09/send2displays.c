#include <detpic32.h>
#include "delay.c"

void send2displays(unsigned char value) {
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
    };
    unsigned int num;

    // select display high
    TRISB = (TRISB & 0xFFC00);
    LATBbits.LATB9 = 0;
    LATBbits.LATB8 = 1;

    num = (value & 0x00F0) >> 4;
    LATB = (LATB & 0xFF00) | display7Scodes[num];

    delay(20);

    // select display low
    TRISB = (TRISB & 0xFFC00);
    LATBbits.LATB9 = 1;
    LATBbits.LATB8 = 0;

    num = value & 0x0F;
    LATB = (LATB & 0xFF00) | display7Scodes[num];

    delay(20);
}
