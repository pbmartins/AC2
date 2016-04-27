#ifndef __DEVICEDRIVER__
#define __DEVICEDRIVER__

#define FALSE                       0
#define TRUE                        1
#define BUF_SIZE                    8
#define INDEX_MASK                  (BUF_SIZE - 1)
#define DisableUart1RxInterrupt()   IEC0bits.U1RXIE = 0
#define EnableUart1RxInterrupt()    IEC0bits.U1RXIE = 1
#define DisableUart1TxInterrupt()   IEC0bits.U1TXIE = 0
#define EnableUart1TxInterrupt()    IEC0bits.U1TXIE = 1

#include <detpic32.h>

typedef struct {
    unsigned char data[BUF_SIZE];
    unsigned int head;
    unsigned int tail;
    unsigned int count;
    unsigned int oerr;
} circularBuffer;

volatile circularBuffer txb;            // Transmission buffer
volatile circularBuffer rxb;            // Reception buffer

/***************************************************/
/* Initialize variables of the reception buffer    */
/***************************************************/
void comDrv_flushRx(void);

/***************************************************/
/* Initialize variables of the transmission buffer */
/***************************************************/
void comDrv_flushTx(void);

/***************************************************/
/* Configures device driver                        */
/***************************************************/
void comDrv_config(unsigned int baud, char parity, unsigned int stopbits);

/***************************************************/
/* Writes a character on the transmission buffer   */
/***************************************************/
void comDrv_putc(char ch);

/***************************************************/
/* Writes a string on the transmission buffer      */
/***************************************************/
void comDrv_puts(char* ch);

/***************************************************/
/* Get a character from reception buffer           */
/***************************************************/
char comDrv_getc(char *pchar);

#endif /* __DEVICEDRIVER__ */
