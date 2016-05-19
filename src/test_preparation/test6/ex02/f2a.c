#include <detpic32.h>
#define ADC_SAMPLES     15

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

    LATB = (LATB & 0xFC00) | 0x0200 | display7Scodes[value];
}

void configADC(void) {
    TRISBbits.TRISB14 = 1; // RB4 digital output disconnected 
    AD1PCFGbits.PCFG14 = 0; // RB4 configured as analog input (AN4)
    AD1CON1bits.SSRC = 7; // Conversion trigger selection bits: in this // modeaninternalcounteren
    AD1CON1bits.CLRASAM = 1; // Stop conversions when the 1st A/D converter
    AD1CON3bits.SAMC = 16; // Sample time is 16 TAD (TAD = 100 ns)
    AD1CON2bits.SMPI = ADC_SAMPLES-1; // Interrupt is generated after XX samples 
    AD1CHSbits.CH0SA = 14; 
    AD1CON1bits.ON = 1;

    IFS1bits.AD1IF = 0;
    IPC6bits.AD1IP = 2;
    IEC1bits.AD1IE = 1;
}

unsigned char convertADC(int *p) {
    int sum = 0, i;
    for (i = 0; i <= ADC_SAMPLES; i++)
        sum += p[i<<2];
    return (sum / (ADC_SAMPLES + 1));
}

void configT2(void) {
    // 25 Hz
    T2CONbits.TCKPS = 5; 
    PR2 = 24999;
    TMR2 = 0; 
    T2CONbits.TON = 1;

    IFS0bits.T2IF = 0;
    IPC2bits.T2IP = 2;
    IEC0bits.T2IE = 1;
}

int main(void) {
    configT2();
    configADC();
    EnableInterrupts();
    TRISE &= 0xF0;

    while (1);
            
    return 0;
}

void _int_(27) isr_adc(void) {
    //send2displays((PORTE & 0x0F0) >> 4);
    int *p = (int *)(&ADC1BUF0);
    LATE = (LATE & 0xF0) | ((convertADC(p) & 0x01E0) >> 6);
    IFS1bits.AD1IF = 0;
}

void _int_(8) isr_T2(void) {
    AD1CON1bits.ASAM = 1;
    IFS0bits.T2IF = 0;
}
