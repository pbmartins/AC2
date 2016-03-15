#include <detpic32.h>
#include "../../lib/initadc.h"

int main(void) {
    unsigned int i, nConvertions = 4;
    int *p;

    initADC(4);                                             // Configure ADC with nConvertions = 4

    while (1) {
        AD1CON1bits.ASAM = 1;                               // Start conversion
        while (IFS1bits.AD1IF == 0);                        // Wait while conversion not done (AD1IF == 0)
        p = (int *)(&ADC1BUF0);
        for (i = 0; i < nConvertions; i++) {
            printInt(p[i * 4], 0x0004000A);
            putChar(' ');
        }
        putChar('\n');
        IFS1bits.AD1IF = 0;                                 // Rest AD1IF
    }

    return 0;
}
