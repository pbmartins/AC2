#include <detpic32.h>
#define CALIBRATION_VALUE 4983

static unsigned char displays[] = {
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
    0b00000000,
    0b01000000
};

static unsigned char displayFlag = 0;

void initADC(void) {
    TRISBbits.TRISB14 = 1;
    AD1PCFGbits.PCFG14 = 0;
    AD1CON1bits.SSRC = 7;
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16;
    AD1CON2bits.SMPI = 1-1; 
    AD1CHSbits.CH0SA = 14;
    AD1CON1bits.ON = 1;
}

int convert2Celsius(int value) {
    return (value * 645 / 10000) - 9;
}

unsigned char toBcd(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}

void send2displays(unsigned char value) {
    unsigned char digit_low = value & 0x0F;
    unsigned char digit_high = value >> 4;

    if (value == 100)
        digit_low = digit_high = 16;

    if (!displayFlag) {
        LATBbits.LATB9 = 1;
        LATBbits.LATB8 = 0;
        LATB = (LATB & 0xF00) | displays[digit_low];
    } else {
        LATBbits.LATB9 = 0;
        LATBbits.LATB8 = 1;
        LATB = (LATB & 0xF00) | displays[digit_high];
    }
    displayFlag = !displayFlag;
}

void delay(unsigned int n_intervals) {
    volatile unsigned int i;
    for(; n_intervals != 0; n_intervals--)
        for(i = CALIBRATION_VALUE; i != 0; i--);
}

int main(void) {
    unsigned int i = 0, d = 0;
    unsigned char toPrint;
    int tmp;
    initADC();
    // Configure displays
    TRISB = TRISB & 0xFC00;
    while (1) {
        delay(10);
        if (i++ == 25) {
            AD1CON1bits.ASAM = 1;
            while (IFS1bits.AD1IF == 0);
            tmp = convert2Celsius(ADC1BUF0);
            i = 0;
            IFS1bits.AD1IF = 0;
        }
        toPrint = tmp;
        
        if (tmp < 0) {
            toPrint = -tmp;
            if (d++ == 50) {
                send2displays(toBcd(toPrint));
                d = 0;
            } else
                send2displays(100);
        } else
            send2displays(toBcd(toPrint));        
    }
    
    return 0;
}
