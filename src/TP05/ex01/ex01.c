#include <detpic32.h>
#include "../../lib/convert2volt.h"
#include "../../lib/displays.h"
#include "../../lib/delay.h"
#include "../../lib/initadc.h"

static unsigned int nConvertions = 1, i;

int main(void) {
    initADC(nConvertions);                                  // Configure ADC with nConvertions = 1
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
    int *p = (int *)(&ADC1BUF0);
    for (i = 0; i < nConvertions; i++) {
        printInt(p[i * 4], 0x00030010);                     // 3bit Hexadecimal
        putChar(' ');
    }
    putChar('\n');
    AD1CON1bits.ASAM = 1;                                   // Star conversion
    IFS1bits.AD1IF = 0;                                     // Rest AD1IF
}