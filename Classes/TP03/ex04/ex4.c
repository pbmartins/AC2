#include <detpic32.h>
#include "delay.c"

int main(void) {
    static const unsigned char codes[] = {
        0x02,                                       // a
        0x01,                                       // b
        0x40,                                       // c
        0x20,                                       // d
        0x10,                                       // e
        0x04,                                       // f
        0x08,                                       // g
        0x80                                        // .
    }

    TRISB = (TRISB & 0xFFC00);                      // configure RB0-RB9 as outputs

    while (1) {
        // select display low
        LATBbits.LATB9 = 1;
        LATBbits.LATB8 = 0;
        for (int i = 0; i < 7; i++) {
            LATB = (LATB & 0xFF00) | codes[i];
            delay(1000);
        }

        // select display high
        LATBbits.LATB9 = 0;
        LATBbits.LATB8 = 1;
        for (int i = 0; i < 7; i++) {
            LATB = (LATB & 0xFF00) | codes[i];
            delay(1000);
        }
    }
    return 0;
}
