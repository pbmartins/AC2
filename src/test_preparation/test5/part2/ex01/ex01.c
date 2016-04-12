#include <detpic32.h>
#define N_CONVERTIONS 4
#define CALIBRATION_VALUE 4998

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

void send2displays(unsigned char value) {
    unsigned char digit_low, digit_high;
    digit_low = value & 0x0F;
    digit_high = value >> 4;

    if (!displayFlag)
        LATB = (LATB & 0xFC00) | 0x0200 | display7Scodes[digit_low];
    else
        LATB = (LATB & 0xFC00) | 0x0100 | display7Scodes[digit_high];

    //displayFlag = !displayFlag;
}

void delay(unsigned int n_intervals) {
    volatile unsigned int i;
    for(; n_intervals != 0; n_intervals--)
        for(i = CALIBRATION_VALUE; i != 0; i--);
}

void initADC(void) {
    TRISBbits.TRISB14 = 1;
    AD1PCFGbits.PCFG14 = 0;

    AD1CON1bits.SSRC = 7;
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16;
    AD1CON2bits.SMPI = N_CONVERTIONS-1;
    AD1CHSbits.CH0SA = 14;
    AD1CON1bits.ON = 1;
}

int average(int* p) {
    unsigned int i;
    int sum = 0;

    for (i = 0; i < N_CONVERTIONS; i++)
        sum += p[i<<2];

    return sum / N_CONVERTIONS;
}

unsigned int convert2hexa(unsigned int value) {
    return (value * 15 + 511) / 1023;
}

int main(void) {
    unsigned int i = 0;
    unsigned int value = 0;
    int* p;

    initADC();
    TRISB = TRISB & 0xFC00;

    while (1) {
        delay(10); // Displays refresh rate = 100Hz
        if (i++ == 2) {
            AD1CON1bits.ASAM = 1;
            while( IFS1bits.AD1IF == 0 );
            p = (int*)(&ADC1BUF0);
            value = convert2hexa(average(p));
            IFS1bits.AD1IF = 0;
            i = 0;
        }
        send2displays(value);
    }


    return 0;
}
