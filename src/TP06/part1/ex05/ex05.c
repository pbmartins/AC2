#include <detpic32.h>
#include "../../../lib/convert2volt.h"
#include "../../../lib/displays.h"
#include "../../../lib/delay.h"
#include "../../../lib/initadc.h"

volatile unsigned char value2displays;
static unsigned int nConvertions = 8;

void configureAll(void) {
    initADC(nConvertions);                                  // Configure ADC with nConvertions = 8
    IFS1bits.AD1IF = 0;                                     // Rest AD1IF
    IPC6bits.AD1IP = 6;                                     // Configure interrupt priority
    IEC1bits.AD1IE = 1;                                     // Enable AD1 Interrupt

    // Configure Timer T1 (4 Hz, interrupts disabled)
    T1CONbits.TCKPS = 7;                                    // 1:256 prescaler (i.e fin = 78125 Hz)
    PR1 = 19530;                                            // Fout = 20MHz / (256 * (19530 + 1)) = 4 Hz
    TMR1 = 0;                                               // Reset timer T1 count register
    T1CONbits.TON = 1;                                      // Enable timer T1 (must be the last command of the timer configuration sequence)

    IFS0bits.T1IF = 0;                                      // Reset timer T1 interrupt flag
    IPC1bits.T1IP = 2;                                      // Interrupt priority (must be in range [1..6])
    IEC0bits.T1IE = 1;                                      // Enable timer T1 interrupts

    // Configure Timer T3 (100 Hz, interrupts disabled)
    T3CONbits.TCKPS = 5;                                    // 1:32 prescaler (i.e fin = 625000 Hz)
    PR3 = 6250;                                             // Fout = 20MHz / (32 * (6250 + 1)) = 100 Hz
    TMR3 = 0;                                               // Reset timer T1 count register
    T3CONbits.TON = 1;                                      // Enable timer T1 (must be the last command of the timer configuration sequence)

    IFS0bits.T3IF = 0;                                      // Reset timer T3 interrupt flag
    IPC3bits.T3IP = 2;                                      // Interrupt priority (must be in range [1..6])
    IEC0bits.T3IE = 1;                                      // Enable timer T3 interrupts

    TRISE = TRISE | 0x30;                                   // Configure RE4 and RE5 as inputs
}

int freeze(void) {
    return (PORTEbits.RE4 == 1) && (PORTEbits.RE5 == 0);
}

int main(void) {
    configureAll();

    EnableInterrupts();                                     // Global Interrupt Enable

    while (1);
    return 0;
}

// Interrupt Handler
void _int_(27) isr_adc(void) {
    int *p = (int *)(&ADC1BUF0);
    value2displays = convert2volt(p, nConvertions);
    IFS1bits.AD1IF = 0;                                     // Rest AD1IF
}

void _int_(4) isr_T1(void) {
    if (!freeze())
        AD1CON1bits.ASAM = 1;                               // Start A/D conversion
    IFS0bits.T1IF = 0;                                      // Reset T1IF flag
}

void _int_(12) isr_T3(void) {
    // Send "value2display" global variable to displays
    send2displays(toBcd(value2displays));
    send2dots(0x0010);
    IFS0bits.T3IF = 0;                                      // Reset T3IF flag
}
