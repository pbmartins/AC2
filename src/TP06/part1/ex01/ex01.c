#include <detpic32.h>

void main(void) {
    // Configure Timer T3 (2 Hz, interrupts disabled)
    T3CONbits.TCKPS = 7;                // 1:32 prescaler (i.e fin = 78125 Hz)
    PR3 = 39061;                        // Fout = 20MHz / (256 * (39061 + 1)) = 2 Hz
    TMR3 = 0;                           // Reset timer T3 count register
    T3CONbits.TON = 1;                  // Enable timer T3 (must be the last command of the timer configuration sequence)

    IEC0bits.T3IE = 0;                  // Disable timer T3 interrupts

    while (1) {
        if (IFS0bits.T3IF == 1) {       // Wait until T3IF == 1
            IFS0bits.T3IF = 0;
            putChar('.');
        }
    }
}
