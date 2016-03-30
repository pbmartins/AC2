#include <detpic32.h>

static unsigned char displayFlag = 0;

void main(void) {
    // Configure Timer T3 (2 Hz, interrupts disabled)
    T3CONbits.TCKPS = 7;                // 1:256 prescaler (i.e fin = 78125 Hz)
    PR3 = 39061;                        // Fout = 20MHz / (256 * (39061 + 1)) = 2 Hz
    TMR3 = 0;                           // Reset timer T3 count register
    T3CONbits.TON = 1;                  // Enable timer T3 (must be the last command of the timer configuration sequence)

    IFS0bits.T3IF = 0;                  // Reset timer T3 interrupt flag
    IPC3bits.T3IP = 2;                  // Interrupt priority (must be in range [1..6])
    IEC0bits.T3IE = 1;                  // Enable timer T3 interrupts

    EnableInterrupts();
    while (1);
}

void _int_(12) isr_T3(void) {
    if (!displayFlag)
        putChar('.');
    displayFlag = !displayFlag;
    IFS0bits.T3IF = 0;                  // Reset timer T3 interrupt flag
}
