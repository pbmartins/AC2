#include <detpic32.h>
#define CALIBRATION_VALUE   4983

static const unsigned char display7Scodes[] = {
        0b01110111,                                // 0
        0b01000001,                                // 1
        0b00111011,                                // 2
        0b01101011,                                // 3
        0b01001101,                                // 4
        0b01101110,                                // 5
        0b01111110,                                // 6
        0b01000011,                                // 7
        0b01111111,                                // 8
        0b01101111,                                // 9
        0b01011111,                                // a
        0b01111100,                                // b
        0b00110110,                                // c
        0b01111001,                                // d
        0b00111110,                                // e
        0b00011110,                                // f
        0b00000000                                 // null
};

static unsigned char toSum = 0;

void delay(unsigned int n_intervals) {
    volatile unsigned int i;
    for(; n_intervals != 0; n_intervals--)
        for(i = CALIBRATION_VALUE; i != 0; i--);
}

void configUART(unsigned int baudrate) {
    U1MODEbits.BRGH = 0;
    U1BRG = ((PBCLK + 8 * baudrate) / (16 * baudrate)) - 1;
    U1MODEbits.PDSEL = 0;
    U1MODEbits.STSEL = 0;
    U1STAbits.UTXEN = 1;
    U1MODEbits.ON = 1;

    U1STAbits.URXISEL = 0;
    IPC6bits.U1IP = 2;
    IFS0bits.U1RXIF = 0;
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

unsigned char toBCD(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}

void send2displays(unsigned char value) {
    unsigned char digit_low = value & 0x0F;
    TRISB &= 0xFC00;
    LATB = (LATB & 0xFC00) | 0x0100 | display7Scodes[digit_low];
}

int main(void) {
    unsigned char value = 0;
    configUART(115200);
    EnableInterrupts();
    TRISE = TRISE | 0x0F0;

    while (1) {
        delay(500);
        value = (((PORTE & 0x0F0) >> 4) + toSum) & 0x0F;
        puts("0x0");
        send2displays(value);
        value < 10 ? putc(value + '0') : putc(value - 10 + 'A');
        putc('\n');
    }
}

void _int_(24) isr_uart1(void) {
    unsigned char tmp = U1RXREG - '0';
    if (tmp <= 9)
        toSum = tmp;
    IFS0bits.U1RXIF = 0;
}
