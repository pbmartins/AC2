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

static unsigned char toSum = 0, displayFlag = 0, toggle = 0, value = 0;

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
    if (displayFlag) 
        LATB = (LATB & 0xFC00) | 0x0100 | display7Scodes[digit_low];
    else
        LATB &= 0xFC00;
}

void configT3() {
    T3CONbits.TCKPS = 7;
    PR3 = 39062;
    TMR3 = 0;
    T3CONbits.TON = 1;

    IFS0bits.T3IF = 0;
    IPC3bits.T3IP = 2;
    IEC0bits.T3IE = 1;
}

int main(void) {
    unsigned char value = 0;
    configUART(115200);
    configT3();
    EnableInterrupts();
    TRISE = TRISE | 0x0F0;

    while (1);
}

void _int_(24) isr_uart1(void) {
    unsigned char tmp = U1RXREG;
    
    if (tmp == 'p')
        toggle = !toggle;
    else if (tmp - '0' <= 9)
        toSum = tmp - '0';
    IFS0bits.U1RXIF = 0;
}

void _int_(12) isr_t3(void) {
    value = (((PORTE & 0x0F0) >> 4) + toSum) & 0x0F;
    puts("0x0");
    send2displays(value);
    if (toggle)
        displayFlag = !displayFlag;
    else
        displayFlag = 1;
    value < 10 ? putc(value + '0') : putc(value - 10 + 'A');
    putc('\n');
    IFS0bits.T3IF = 0;
}
