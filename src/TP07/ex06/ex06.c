#include <detpic32.h>
#include "../../lib/delay.h"
#include "../../lib/configuart.h"

void putc(char byte2send) {
    while (U1STAbits.UTXBF == 1);
    U1TXREG = byte2send;
}

void puts(char *str) {
    char c;
    while ((c = *str++) != 0)
        putc(c);
}

int main(void) {
    configUART(115200, 'N', 1);
    TRISEbits.TRISE0 = 0;

    while (1) {
        while (!U1STAbits.TRMT);
        LATEbits.LATE0 = 1;
        // puts("12345");
        // puts("123456789");
        // puts("123456789A");
        puts("123456789AB");
        LATEbits.LATE0 = 0;
    }

}

