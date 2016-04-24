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

char getc(void) {
    char rtn;
    if (U1STAbits.OERR)
        U1STAbits.OERR = 0;
    while (!U1STAbits.URXDA);
    if (U1STAbits.FERR || U1STAbits.PERR) {
        rtn = U1RXREG;
        return 0;
    }
    return rtn;
}

int main(void) {
    configUART(115200, 'N', 1);

    while (1) {
        putc(getc());
    }

}

