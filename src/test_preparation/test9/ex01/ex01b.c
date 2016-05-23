#include <detpic32.h>

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

void send2displays(unsigned char value) {
    TRISB &= 0xFC00;
    LATB = (LATB & 0xFC00) | 0x0100 | display7Scodes[value];
}

int main(void) {
    unsigned char value;
    TRISE = (TRISE & 0xF00) | 0x0F0;

    while (1) {
        value = (PORTE & 0x0F0) >> 4;
        send2displays(value);
        LATEbits.LATE0 = value >> 3;
        LATEbits.LATE1 = (value >> 2) & 0x01;
        LATEbits.LATE2 = (value >> 1) & 0x01;
        LATEbits.LATE3 = value & 0x01;
    }
    return 0;
}
