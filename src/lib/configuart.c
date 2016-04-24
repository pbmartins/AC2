#include "configuart.h"

/**********************************************/
/* Configures UART1                           */
/**********************************************/
void configUART(unsigned int baud, char parity, unsigned int stopbits) {
    // Configure UART1:
    // Configure BaudRate Generator
    if (baud < 600 || baud > 115200)
        baud = 115200;
    U1MODEbits.BRGH = 0;
    U1BRG = ((PBCLK + 8 * baud) / (16 * baud)) - 1;

    // Configure number of data bits, parity and number of stop bits (see U1MODE register)
    switch (parity) {
        case 'N': default: U1MODEbits.PDSEL = 0; break;
        case 'E': U1MODEbits.PDSEL = 1; break;
        case 'O': U1MODEbits.PDSEL = 2; break;
    }

    U1MODEbits.STSEL = 0;
    if (stopbits == 2)
        U1MODEbits.STSEL = 1;

    // Enable the trasmitter and receiver modules (see register U1STA)
    U1STAbits.UTXEN = U1STAbits.URXEN = 1;
    // Enable UART1 (see register U1MODE)
    U1MODEbits.ON = 1;

}

/**********************************************/
/* Configures UART1 Interrupts                */
/* 0 -> Receiver only                         */
/* 1 -> Transmiter only                       */
/* 2 -> Both                                  */
/**********************************************/
void enableUARTInterrupts(int config) {
    switch (config) {
        case 0:
            U1STAbits.URXISEL = 0;
            IFS0bits.U1RXIF = 0;
            IPC6bits.U1IP = 6;
            IEC0bits.U1RXIE = 1;
            IEC0bits.U1EIE = 1;
            break;
        case 1:
            //U1STAbits.UTXISEL = 0;
            IFS0bits.U1TXIF = 0;
            IPC6bits.U1IP = 6;
            IEC0bits.U1TXIE = 1;
            IEC0bits.U1EIE = 1;
            break;
        case 2:
            //U1STAbits.UTXISEL = 0;
            U1STAbits.URXISEL = 0;
            // Interrupt flag reset
            IFS0bits.U1TXIF = 0;
            IFS0bits.U1RXIF = 0;
            // Configure interrupts priority
            IPC6bits.U1IP = 6;
            // Enable interrupts on UART1
            IEC0bits.U1TXIE = 1;
            IEC0bits.U1RXIE = 1;
            IEC0bits.U1EIE = 1;
            break;
        
        default:
            break;
    }
}
