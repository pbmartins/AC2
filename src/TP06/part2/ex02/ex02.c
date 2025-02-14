#include <detpic32.h>

void configureAll(void) {
    // Configure Timer T3 (100 Hz, interrupts disabled)
    T3CONbits.TCKPS = 5;                                    // 1:32 prescaler (i.e fin = 625000 Hz)
    PR3 = 6250;                                             // Fout = 20MHz / (32 * (6250 + 1)) = 100 Hz
    TMR3 = 0;                                               // Reset timer T1 count register
    T3CONbits.TON = 1;                                      // Enable timer T1 (must be the last command of the timer configuration sequence)


    IFS0bits.T3IF = 0;                                      // Reset timer T3 interrupt flag
    IPC3bits.T3IP = 2;                                      // Interrupt priority (must be in range [1..6])
    IEC0bits.T3IE = 1;                                      // Enable timer T3 interrupts
}

void setPWM(unsigned int dutyCycle) {
    // dutyCycle must be in the range [0, 100]
    if (dutyCycle < 0 || dutyCycle > 100)
        return;

    OC1CONbits.OCM = 6;                                     // PWM mode on OC1; fault pin disabled 
    OC1CONbits.OCTSEL = 1;                                  // Use timer T3 as the time base for PWM generation
    OC1RS = ((PR3 + 1) * dutyCycle) / 100;                  // Evaluate OC1RS as a function of "dutyCycle"
    OC1CONbits.ON = 1;                                      // Enable OC1 module
}

int main(void) {
    configureAll();
    setPWM(100);
    EnableInterrupts();
    while (1);
    return 0;
}
