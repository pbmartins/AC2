#include <detpic32.h>
#include "../../lib/devicedriver.h"

int main(void) {
    comDrv_config(115200,'N',1);            /* default "pterm" parameters with TX 
                                                and RX interrupts disabled */
    comDrv_flushRx();
    comDrv_flushTx();
    EnableInterrupts();

    while(1)
        comDrv_puts("Teste do bloco de transmissao do device driver ");
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

}
