#include <detpic32.h>
#include "../../lib/convert2volt.h"
#include "../../lib/displays.h"
#include "../../lib/delay.h"
#include "../../lib/initadc.h"

volatile unsigned char value2displays;
static unsigned int nConvertions = 8;

int main(void) {
    unsigned int c = 0;
    initADC(nConvertions);                                  // Configure ADC with nConvertions = 8
    IFS1bits.AD1IF = 0;                                     // Rest AD1IF
    IPC6bits.AD1IP = 6;                                     // Configure interrupt priority
    IEC1bits.AD1IE = 1;                                     // Enable AD1 Interrupt
    EnableInterrupts();                                     // Global Interrupt Enable


    while (1) {
        delay(10);                                          // Wait 10 ms using the core timer
        if (c++ == 25) {
            AD1CON1bits.ASAM = 1;                           // Start conversion
            c = 0;
        }
        send2displays(toBcd(value2displays));
        send2dots(0x0010);
    }
    return 0;
}

// Interrupt Handler
void _int_(27) isr_adc(void) {
    int *p = (int *)(&ADC1BUF0);
    value2displays = convert2volt(p, nConvertions);
    IFS1bits.AD1IF = 0;                                     // Rest AD1IF
}
