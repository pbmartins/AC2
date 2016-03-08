#include <detpic32.h>
#include "../../lib/convert2volt.c"
#include "../../lib/send2displays.c"
#include "../../lib/delay.c"

static unsigned int nConvertions = 1, i;

int main(void) {
    TRISEbits.TRISE0 = 0;                                   // Configure R04 as output
    TRISBbits.TRISB14 = 1;                                  // RB14 digital output disconnected
    AD1PCFGbits.PCFG14 = 0;                                 // RB14 configured as analog input (AN4)
    AD1CHSbits.CH0SA = 14;                                  // Selects AN14 as input for the A/D converter

    AD1CON1bits.SSRC = 7;                                   // Conversion trigger selection bits: in this mode an internal counter ends sampling and starts conversion
    AD1CON1bits.CLRASAM = 1;                                // Stop conversions when the 1st A/D converter interrupt is generated. At the same time, hardware clears the ASAMbit
    AD1CON3bits.SAMC = 16;                                  // Sample time is 16 TAD (TAD = 100 ns)
    AD1CON2bits.SMPI = nConvertions - 1;                    // Interrupt is generated after XX samples, replace XX by the desired number of consecutive samples

    AD1CON1bits.ON = 1;                                     // Enable A/D converter. This must the last command of the A/D configuration sequence

    IFS1bits.AD1IF = 0;                                     // Rest AD1IF
    IPC6bits.AD1IP = 6;                                     // Configure interrupt priority
    IEC1bits.AD1IE = 1;                                     // Enable AD1 Interrupt
    EnableInterrupts();                                     // Global Interrupt Enable
    AD1CON1bits.ASAM = 1;                                   // Star conversion

    while (1);
    return 0;
}

// Interrupt Handler
void _int_(27) isr_adc(void) {
    LATEbits.LATE0 = 0;                                     // Reset RE0
    int *p = (int *)(&ADC1BUF0);
    for (i = 0; i < nConvertions; i++) {
        printInt(p[i * 4], 0x00030010);                     // 3bit Hexadecimal
        putChar(' ');
    }
    putChar('\n');
    LATEbits.LATE0 = 1;                                     // Set RE0
    AD1CON1bits.ASAM = 1;                                   // Star conversion
    IFS1bits.AD1IF = 0;                                     // Rest AD1IF
}