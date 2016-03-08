#include "delay.c"
#include <detpic32.h>

int main(void) {
    TRISEbits.TRISE0 = 0;
    while (1) {
        LATEbits.LATE0 = 0;
        delay(1);
        LATEbits.LATE0 = 1;
        delay(1);
    }
    return 0;
}