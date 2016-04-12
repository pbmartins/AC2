#include <detpic32.h>
#define CALIBRATION_VALUE 4983

static const unsigned char display7Scodes[] = {0x01, 0x01};
static const unsigned char snakeCodes[] = {
    0b00000100,
    0b00000010,
    0b00000001,
    0b00001000,
    0b00010000,
    0b00100000,
    0b01000000,
    0b00001000
};
static unsigned char displayFlag = 1;

void send2displays(unsigned char value) {
    unsigned char digit_low, digit_high;
    digit_low = value & 0x0F;
    digit_high = value >> 4;

    if (!displayFlag)
        LATB = (LATB & 0xFC00) | 0x0200 | snakeCodes[digit_low];
    else
        LATB = (LATB & 0xFC00) | 0x0100 | snakeCodes[digit_high];

    //displayFlag = !displayFlag;
}

void delay(unsigned int n_intervals) {
    volatile unsigned int i;
    for(; n_intervals != 0; n_intervals--)
        for(i = CALIBRATION_VALUE; i != 0; i--);
}

int main(void) {
    unsigned int i;
    unsigned char c = 0;

    // Configure displays as outputs
    TRISB = TRISB & 0xFC00;

    while (1) {
        i = 0;
        do {
            delay(50);
            send2displays(c<<4);
        } while (++i < 5);
        if (++c >= 8)
            c = 0;
    }
    return 0;
}
