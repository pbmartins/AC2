#include <detpic32.h>
#include "../../lib/convert2volt.h"
#include "../../lib/displays.h"
#include "../../lib/delay.h"
#include "../../lib/initadc.h"

volatile unsigned char value2displays = 0;

void configureAll(void) {
    unsigned int freq;
    // Read value of frequency
    printStr("Frequency value: ");
    freq = readInt10();
    putChar('\n');  
 
    // Configure Timer T1 (100 Hz, interrupts disabled)
    T1CONbits.TCKPS = 2;                                    // 1:8 prescaler (i.e fin = 2.5 MHz)
    PR1 = 24999;                                            // Fout = 20MHz / (8 * (24999 + 1)) = 100 Hz
    TMR1 = 0;                                               // Reset timer T1 count register
    T1CONbits.TON = 1;                                      // Enable timer T1 (must be the last command of the timer configuration sequence)

    IFS0bits.T1IF = 0;                                      // Reset timer T1 interrupt flag
    IPC1bits.T1IP = 2;                                      // Interrupt priority (must be in range [1..6])
    IEC0bits.T1IE = 1;  
 
    // Configure Timer T3 (100 Hz, interrupts disabled)
    T3CONbits.TCKPS = 5;                                    // 1:32 prescaler (i.e fin = 625000 Hz)
    PR3 = (625000 / freq) - 1;                              // Fout = 20MHz / (32 * (PR3 + 1)) = freq Hz
    TMR3 = 0;                                               // Reset timer T3 count register
    T3CONbits.TON = 1;                                      // Enable timer T3 (must be the last command of the timer configuration sequence)
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
    EnableInterrupts();
    unsigned char c;

    printStr("Use '+' or '-' to change LED brightness: ");
    while (c = getChar()) {
        if (c == '+') {
            if (value2displays != 100)
                value2displays += 10;
        } else if (c == '-') {
            if (value2displays != 0)
                value2displays -= 10;
        } else
            continue;
        setPWM(value2displays);
    }
    return 0;
}

void _int_(4) isr_T1(void) {
    // Send "value2display" global variable to displays
    send2displays(toBcd(value2displays));
    IFS0bits.T1IF = 0;                                      // Reset T3IF flag
}
