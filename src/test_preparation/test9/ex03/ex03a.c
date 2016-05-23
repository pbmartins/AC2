#include <detpic32.h>

void configUART(unsigned int baudrate) {
    U1MODEbits.BRGH = 0;
    U1BRG = ((PBCLK + 8 * baudrate) / (16 * baudrate)) - 1;
    U1MODEbits.PDSEL = 0;
    U1MODEbits.STSEL = 0;
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 1;
    U1MODEbits.ON = 1;

    U1STAbits.URXISEL = 0;
    IFS0bits.U1TXIF = 0;
    IFS0bits.U1RXIF = 0;
    IFS0bits.U1EIF = 0;
    IPC6bits.U1IP = 2;
    IEC0bits.U1RXIE = 1;
    IEC0bits.U1EIE = 1;
}

void putc(char byte2send) {
    while (U1STAbits.UTXBF);
    U1TXREG = byte2send;
}

void puts(char* str) {
    while (*str)
        putc(*str++);
}

char getc(void) {
    if (U1STAbits.OERR)
        U1STAbits.OERR = 0;
    while (!U1STAbits.URXDA);
    return U1RXREG;
}

int main(void) {
    configUART(115200);
    EnableInterrupts();
    TRISE |= 0x0F0;

    while (1);
    return 0;
}

void _int_(24) isr_uart1(void) {
    unsigned char value;
    if (IFS0bits.U1EIF) {
        if (U1STAbits.OERR)
            U1STAbits.OERR = 0;
        else
            value = U1RXREG;
        IFS0bits.U1EIF = 0;
    }

    if (IFS0bits.U1RXIF) {
        value = U1RXREG;
        if (value == 'l') {
            if (PORTEbits.RE4)
                puts("The value in the switches is odd.\n");
            else
                puts("The value in the switches is even.\n");
        } else if (value == 's')
            puts("Pedro Martins - 76551\n");
        else if (value == 'q')
            exit(0);

        IFS0bits.U1RXIF = 0;
    }
}
