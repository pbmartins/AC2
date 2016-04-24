#include <detpic32.h>
#include "../../lib/delay.h"
#define fPBCLK 20000000

void initUART(void) {
    // Configure UART1:
    // Configure BaudRate Generator
    unsigned int baudrate = 115200;
    U1MODEbits.BRGH = 0;
    U1BRG = ((fPBCLK + 8 * baudrate) / (16 * baudrate)) - 1;
    // Configure number of data bits, parity and number of stop bits (see U1MODE register)
    U1MODEbits.PDSEL = 0;
    U1MODEbits.STSEL = 0;
    // Enable the trasmitter and receiver modules (see register U1STA)
    U1STAbits.UTXEN = U1STAbits.URXEN = 1;
    // Enable UART1 (see register U1MODE)
    U1MODEbits.ON = 1;
}

void putc(char byte2send) {
    while (U1STAbits.UTXBF == 1);
    U1TXREG = byte2send;
}

void puts(char *str) {
    char* p = str;
    char c;
    while ((c = *p++) != 0)
        putc(c);
}

int main(void) {
    initUART();

    while (1) {
        puts("String de teste\n");
        delay(1000);
    }

}

