#ifndef __CONFIGUART__
#define __CONFIGUART__

#include <detpic32.h>

/**********************************************/
/* Configures UART1                           */
/**********************************************/
void configUART(unsigned int baud, char parity, unsigned int stopbits);

/**********************************************/
/* Configures UART1 Interrupts                */
/* 0 -> Receiver only                         */
/* 1 -> Transmiter only                       */
/* 2 -> Both                                  */
/**********************************************/
void enableUARTInterrupts(int config);

#endif /* __CONFIGUART__ */
