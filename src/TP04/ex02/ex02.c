#include <detpic32.h>
#include "../../lib/initadc.h"

int main(void) {
    initADC(1);                                             // Configure ADC with nConvertions = 1
    while (1) {
        AD1CON1bits.ASAM = 1;                               // Start conversion
        while (IFS1bits.AD1IF == 0);                        // Wait while conversion not done (AD1IF == 0)
        printInt(ADC1BUF0, 0x00030010);
        putChar('\n');
        IFS1bits.AD1IF = 0;                                 // Rest AD1IF
    }

    return 0;
}