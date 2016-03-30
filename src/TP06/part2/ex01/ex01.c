#include <detpic32.h>

int main(void) {
    configureAll();
    EnableInterrupts();
    while (1);
    return 0;
}

void configureAll(void) {
    // Configure Timer T3 (100 Hz, interrupts disabled)
    T3CONbits.TCKPS = 5;                                    // 1:32 prescaler (i.e fin = 625000 Hz)
    PR3 = 6250;                                             // Fout = 20MHz / (32 * (6250 + 1)) = 100 Hz
    TMR3 = 0;                                               // Reset timer T1 count register
    T3CONbits.TON = 1;                                      // Enable timer T1 (must be the last command of the timer configuration sequence)

    OC1CONbits.OCM = 6;                                     // PWM mode on OC1; fault pin disabled 
    OC1CONbits.OCTSEL = 1;                                  // Use timer T3 as the time base for PWM generation
    OC1RS = 1250;                                           // Ton constant
    OC1CONbits.ON = 1;                                      // Enable OC1 module

    IFS0bits.T3IF = 0;                                      // Reset timer T3 interrupt flag
    IPC3bits.T3IP = 2;                                      // Interrupt priority (must be in range [1..6])
    IEC0bits.T3IE = 1;                                      // Enable timer T3 interrupts
}
