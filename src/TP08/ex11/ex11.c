#include <detpic32.h>
#include "../../lib/devicedriver.h"

int main(void) {
    char ch;
    comDrv_config(115200,'N',1);            /* default "pterm" parameters with TX 
                                                disabled and RX interrupts enabled */
    EnableUart1RxInterrupt();
    comDrv_flushRx();
    comDrv_flushTx();
    EnableInterrupts();

    comDrv_puts("PIC32 UART Device-driver\n");

    while(1) {
        if (comDrv_getc(&ch)) {
            if (ch == 'S')
                comDrv_puts("Universidade de Aveiro - DETI");
            else
                comDrv_putc(ch);
        }
    }
    return 0;
}

void _int_(24) isr_uart1(void) {
    if (IFS0bits.U1TXIF) {
        if (txb.count) {
            U1TXREG = txb.data[txb.head];   /* Copy character pointed
                                                by "head" to U1TXREG register */
            txb.head++;                     // Increment "head" variable
            txb.head &= INDEX_MASK;         // (mod BUF_SIZE)
            txb.count--;                    // Decrement "count" variable
        }
        if (!txb.count)
            DisableUart1TxInterrupt();
        IFS0bits.U1TXIF = 0;
    }

    if (IFS0bits.U1RXIF) {
        rxb.data[rxb.tail] = U1RXREG;       /* Read character from UART and
                                                write it to the "tail" position
                                                of the reception buffer */
        rxb.tail++;                         // Increment "tail" variable
        rxb.tail &= INDEX_MASK;             // (mod BUF_SIZE)
        
        if (rxb.count < BUF_SIZE)           // If buffer is not full
            rxb.count++;
        else {
            rxb.head++;                     // Increment "head" variable
            rxb.head &= INDEX_MASK;         // (discard oldest character)
        }
        IFS0bits.U1RXIF = 0;
    }
}
