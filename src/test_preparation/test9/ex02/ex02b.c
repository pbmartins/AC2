#include <detpic32.h>
#define N_SAMPLES   15

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

static unsigned char displayFlag = 0, value2display = 0, i = 0;

void send2displays(unsigned char value) {
    unsigned char digit_low, digit_high;
    digit_low = value & 0x0F;
    digit_high = value >> 4;
    TRISB &= 0xFC00;

    if (!displayFlag)
        LATB = (LATB & 0xFC00) | 0x200 | display7Scodes[digit_low];
    else
        LATB = (LATB & 0xFC00) | 0x100 | display7Scodes[digit_high];

    displayFlag = !displayFlag;
}

unsigned char toBCD(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}

void configADC(void) {
    TRISBbits.TRISB14 = 1;
    AD1PCFGbits.PCFG14 = 0;

    AD1CON1bits.SSRC = 7;
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16;
    AD1CON2bits.SMPI = N_SAMPLES-1; 
    AD1CHSbits.CH0SA = 14;
    AD1CON1bits.ON = 1;

    IFS1bits.AD1IF = 0;
    IPC6bits.AD1IP = 2;
    IEC1bits.AD1IE = 1;
}

unsigned int convertADC(int *p) {
    int sum = 0, i;
    for( i = 0; i <= N_SAMPLES; i++ )
        sum += p[i<<2];
    return ((sum / N_SAMPLES) * 99 + 511) / 1023;
}

void configT2(void) {
    // 120Hz
    T2CONbits.TCKPS = 7;
    PR2 = 650;
    TMR2 = 0;
    T2CONbits.TON = 1;

    IFS0bits.T2IF = 0;
    IPC2bits.T2IP = 2;
    IEC0bits.T2IE = 1;
}

void configOC1(unsigned int duty_cycle) {
    OC1CONbits.OCM = 6; 
    OC1CONbits.OCTSEL =0;
    OC1RS = ((PR2 + 1) * duty_cycle) / 100;
    OC1CONbits.ON = 1;
}

int main(void) {
    configADC();
    configT2();
    configOC1(75);
    EnableInterrupts();

    while (1);
}

void _int_(27) isr_adc(void) {
    int *p = (int *)(&ADC1BUF0);
    value2display = convertADC(p);
    IFS1bits.AD1IF = 0;
}

void _int_(8) isr_t2(void) {
    send2displays(toBCD(value2display));
    if (++i >= 12) {
        AD1CON1bits.ASAM = 1;
        i = 0;
    }
    IFS0bits.T2IF = 0;
}
