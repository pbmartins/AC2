#include <detpic32.h>
#include "../../lib/convert2volt.h"
#include "../../lib/displays.h"
#include "../../lib/delay.h"
#include "../../lib/initadc.h"

int main(void) {
    unsigned int i = 0, voltage;
    int *p = (int *)(&ADC1BUF0);

    initADC(4);                                             // Configure ADC with nConvertions = 4

    while (1) {
        delay(10);                                          // delay of 10 ms of displays refresh rate
        if (i++ == 25) {
            AD1CON1bits.ASAM = 1;                           // Start conversion
            while (IFS1bits.AD1IF == 0);                    // Wait while conversion not done (AD1IF == 0)
            voltage = convert2volt(p, nConvertions);
            IFS1bits.AD1IF = 0;                             // Rest AD1IF
            i = 0;
        }
        send2displays(toBcd(voltage));
        send2dots(0x0010);
    }

    return 0;
}