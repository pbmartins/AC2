#include <detpic32.h>
#include "../../lib/convert2volt.h"
#include "../../lib/initadc.h"

int main(void) {
    int *p;

    initADC(4);                                             // Configure ADC with nConvertions = 4

    while (1) {
        AD1CON1bits.ASAM = 1;                               // Start conversion
        while (IFS1bits.AD1IF == 0);                        // Wait while conversion not done (AD1IF == 0)
        p = (int *)(&ADC1BUF0);
        printInt(convert2volt(p, nConvertions), 10);
        putChar('\n');
        IFS1bits.AD1IF = 0;                                 // Reset AD1IF
    }

    return 0;
}