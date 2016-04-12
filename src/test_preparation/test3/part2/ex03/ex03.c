#include <detpic32.h>
#define CALIBRATION_VALUE 4983
#define N_CONVERTIONS 4

static const unsigned char display7Scodes[] = {0x01, 0x01};
static const unsigned char snakeCodes[] = {
    0b00000100,
    0b00000010,
    0b00000001,
    0b00010000,
    0b00100000,
    0b01000000,
    0b00000001,
    0b00000010,
    0b00000100,
    0b01000000,
    0b00100000,
    0b00010000,
};
static const unsigned int speedValues[] = {
    50,
    100,
    150,
    200,
    250,
    300,
    350,
    400
};
static unsigned char displayFlag = 1, c = 0;

void send2displays(unsigned char value) {
    if (c < 3 || c > 8)
        displayFlag = 1;
    else
        displayFlag = 0;

    printInt10(value);
    putChar('\n');

    if (!displayFlag)
        LATB = (LATB & 0xFC00) | snakeCodes[value] | 0x0200;
    else
        LATB = (LATB & 0xFC00) | snakeCodes[value] | 0x0100;

}

int average(int *p) {
    unsigned int i;
    int sum = 0;
    for( i = 0; i < N_CONVERTIONS; i++ )
        sum += p[i<<2];
    return sum / N_CONVERTIONS;
}

unsigned int convert2speed(unsigned int value) {
    return (value * 7) / 1023;
}

void delay(unsigned int n_intervals) {
    volatile unsigned int i;
    for(; n_intervals != 0; n_intervals--)
        for(i = CALIBRATION_VALUE; i != 0; i--);
}

void initADC(void) {
    TRISBbits.TRISB14 = 1; // RB14 digital output disconnected
    AD1PCFGbits.PCFG14 = 0; // RB14 configured as analog input (AN4)

    AD1CON1bits.SSRC = 7;
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16;
    AD1CON2bits.SMPI = N_CONVERTIONS-1;
    AD1CHSbits.CH0SA = 14;
    AD1CON1bits.ON = 1;
}

int main(void) {
    unsigned int i = 0, speed = 0, speedCounter = 0;

    initADC();
    // Configure displays as outputs
    TRISB = TRISB & 0xFC00;

    while (1) {
        delay(5);
        if (i++ == 4) {
            AD1CON1bits.ASAM = 1;
            while( IFS1bits.AD1IF == 0 );
            int *p = (int *)(&ADC1BUF0);
            speed = convert2speed(average(p));
            i = 0;
            IFS1bits.AD1IF = 0;
        }
        
        if (speedCounter++ >= speedValues[speed]) {
            if (++c >= 12)
                c = 0;
            speedCounter = 0;
        }
        send2displays(c);

    }
    return 0;
}
