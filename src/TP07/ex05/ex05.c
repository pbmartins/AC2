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
    configUART(115200, 'E', 1);

    while (1) {
        puts("String de teste\n");
        delay(1000);
    }

}

