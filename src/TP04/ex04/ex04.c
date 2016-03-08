#include <detpic32.h>

int main(void) {
    unsigned int nConvertions = 4, i;
    int *p;

    TRISBbits.TRISB14 = 1;                                  // RB14 digital output disconnected
    AD1PCFGbits.PCFG14 = 0;                                 // RB14 configured as analog input (AN4)
    AD1CHSbits.CH0SA = 14;                                  // Selects AN14 as input for the A/D converter
    AD1CON1bits.SSRC = 7;                                   // Conversion trigger selection bits: in this mode an internal counter ends sampling and starts conversion
    AD1CON1bits.CLRASAM = 1;                                // Stop conversions when the 1st A/D converter interrupt is generated. At the same time, hardware clears the ASAMbit
    AD1CON3bits.SAMC = 16;                                  // Sample time is 16 TAD (TAD = 100 ns)
    AD1CON2bits.SMPI = nConvertions - 1;                                 // Interrupt is generated after XX samples, replace XX by the desired number of consecutive samples
    AD1CON1bits.ON = 1;                                     // Enable A/D converter. This must the last command of the A/D configuration sequence

    while (1) {
        AD1CON1bits.ASAM = 1;                               // Start conversion
        while (IFS1bits.AD1IF == 0);                        // Wait while conversion not done (AD1IF == 0)
        p = (int *)(&ADC1BUF0);
        for (i = 0; i < nConvertions; i++) {
            printInt(p[i * 4], 0x0004000A);
            putChar(' ');
        }
        putChar('\n');
        IFS1bits.AD1IF = 0;                                 // Rest AD1IF
    }

    return 0;
}