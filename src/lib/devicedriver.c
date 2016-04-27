#include "devicedriver.h"

/***************************************************/
/* Initialize variables of the reception buffer ****/
/***************************************************/
void comDrv_flushRx(void) {
    rxb.head = rxb.tail = rxb.count = 0 = rxb.oerr = 0;
}

/***************************************************/
/* Initialize variables of the transmission buffer */
/***************************************************/
void comDrv_flushTx(void) {
    txb.head = txb.tail = txb.count = 0;
}

/***************************************************/
/* Configures device driver                        */
/***************************************************/
void comDrv_config(unsigned int baud, char parity, unsigned int stopbits) {
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

    // Configure Reception interrupts
    U1STAbits.URXISEL = 0;
    IFS0bits.U1RXIF = 0;
    IPC6bits.U1IP = 6;
    IEC0bits.U1RXIE = 0;

    // Configure Transmission interrupts
    IFS0bits.U1TXIF = 0;
    IEC0bits.U1TXIE = 0;

}

/***************************************************/
/* Writes a character on the transmission buffer   */
/***************************************************/
void comDrv_putc(char ch) {
    while (txb.count == BUF_SIZE);              // Wait while buffer is full
    txb.data[txb.tail] = ch;                    /* Copy character to the transmission 
                                                    buffer at position "tail" */
    txb.tail = (txb.tail + 1) & INDEX_MASK;     // Increment "tail" index (mod.BUF_SIZE)
    DisableUart1TxInterrupt();                  // Begin of critical section
    txb.count++;                                // Increment "count" variable
    EnableUart1TxInterrupt();                   // End of critical section
}

/***************************************************/
/* Writes a string on the transmission buffer   */
/***************************************************/
void comDrv_puts(char* ch) {
    while (*ch)
        comDrv_putc(*ch++);
}

/***************************************************/
/* Get a character from reception buffer           */
/***************************************************/
char comDrv_getc(char *pchar) {
    if (!rxb.count)                             // Test "count" variable (reception buffer)
        return FALSE;                           // and return FALSE if it's zero

    DisableUart1RxInterrupt();                  // Begin of critical section
    *pchar = rxb.data[rxb.head];                // Copy character pointed by "head" to *pchar
    rxb.count--;                                // Decrement "count" variable
    rxb.head++;                                 // Increment "head" variable
    rxb.head &= INDEX_MASK;                     // (mod BUF_SIZE)
    EnableUart1RxInterrupt();                   // End of critical section
    return TRUE;
}
