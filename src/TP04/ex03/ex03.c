#include <detpic32.h>
#include "../../lib/initadc.h"

int main(void) {
    TRISEbits.TRISE0 = 0;                                   // Configure RE0 to check conversion times
    initADC(2);                                             // Configure ADC with nConvertions = 2

    unsigned int aux;

    while (1) {
        LATEbits.LATE0 = 0;
        AD1CON1bits.ASAM = 1;                               // Start conversion
        while (IFS1bits.AD1IF == 0);                        // Wait while conversion not done (AD1IF == 0)
        LATEbits.LATE0 = 1;
        aux = ADC1BUF0;
        aux = ADC1BUF1;
        IFS1bits.AD1IF = 0;                                 // Reset AD1IF
    }

    return 0;
}