#include <detpic32.h>
#define CALIBRATION_VALUE   4983

void delay(unsigned int n_intervals) {
    volatile unsigned int i;
    for(; n_intervals != 0; n_intervals--)
        for(i = CALIBRATION_VALUE; i != 0; i--);
}

void configUART(unsigned int baudrate) {
    U1MODEbits.BRGH = 0; // 16
    U1BRG = ((PBCLK + 8 * baudrate) / (16 * baudrate)) - 1;
    U1MODEbits.PDSEL = 0;
    U1MODEbits.STSEL = 0;
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 0;
    U1MODEbits.ON = 1;
}

void putc(char byte2send) {
    while (U1STAbits.UTXBF);
    U1TXREG = byte2send;
}

unsigned char toBCD(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}

int main(void) {
    TRISE |= 0x0F0;
    unsigned char value = 0, i = 0;
    configUART(115200);

    while (1) {
        delay(10);
        value = toBCD((PORTE & 0x0F0) >> 4);
        if (++i >= 100) {
            putc(((value >> 4) & 0x0F) + '0');
            putc((value & 0x0F) + '0');
            i = 0;
        }
    }
}
