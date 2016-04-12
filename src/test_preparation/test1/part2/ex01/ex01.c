#include <detpic32.h>

static unsigned char leds = {0x000, 0x001, 0x003, 0x007, 0x00F, 0x01F, 0x03F, 0x07F};

void initADC(void) {
    TRISBbits.TRISB4 = 1;
    AD1PCFGbits.PCFG4 = 0;
    AD1CON1bits.SSRC = 7;
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16;
    AD1CON2bits.SMPI = 1-1; 
    AD1CHSbits.CH0SA = 4;
    AD1CON1bits.ON = 1;
}

int main(void) {
    unsigned int nBits;
    initADC();
    // Configure LEDS
    TRISE = TRISE & 0xF0;
    while (1) {
        AD1CON1bits.ASAM = 1;
        while (IFS1bits.AD1IF == 0);
        nBits = ADC1BUF0 / 128;
        LATE = (LATE & 0xF0) | leds[nBits];
        IFS1bits.AD1IF = 0;
    }
    
    return 0;
}
