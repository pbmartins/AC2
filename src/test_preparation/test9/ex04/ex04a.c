#include <detpic32.h>
#define N_SAMPLES   15
#define BUF_SIZE    8
#define INDEX_MASK  (BUF_SIZE - 1)
#define DisableUart1RxInterrupt()   IEC0bits.U1RXIE = 0
#define EnableUart1RxInterrupt()    IEC0bits.U1RXIE = 1
#define DisableUart1TxInterrupt()   IEC0bits.U1TXIE = 0
#define EnableUart1TxInterrupt()    IEC0bits.U1TXIE = 1

typedef struct {
    unsigned char data[BUF_SIZE];
    unsigned int head;
    unsigned int tail;
    unsigned int count;
} circularBuffer;

volatile circularBuffer txb;
volatile circularBuffer rxb;

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

static unsigned char displayFlag = 0, value2display = 0, i = 0, flag = 0;
static unsigned int c = 0;

void comDrv_flushRx(void) {
    txb.head = txb.tail = txb.count = 0;
}

void comDrv_flushTx(void) {
    txb.head = txb.tail = txb.count = 0;
}

void comDrv_config(unsigned int baudrate) {
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
    IEC0bits.U1TXIE = 0;
    IEC0bits.U1RXIE = 0;
    IEC0bits.U1EIE = 0;
}

void comDrv_putc(char ch) {
    while(txb.count == BUF_SIZE); 
    txb.data[txb.tail] = ch; 
    txb.tail = (txb.tail + 1) & INDEX_MASK;
    DisableUart1TxInterrupt();
    txb.count++;
    EnableUart1TxInterrupt();
}

void comDrv_puts(char *str) {
    while (*str)
        comDrv_putc(*str++);
}

char comDrv_getc(char *pchar) {
    if (!rxb.count)
        return 0;
    DisableUart1RxInterrupt(); 
    *pchar = rxb.data[rxb.head];
    rxb.count--;
    rxb.head = (rxb.head + 1) & INDEX_MASK;
    EnableUart1RxInterrupt();
    return 1;
}

void send2displays(unsigned char value) {
    unsigned char digit_low, digit_high;
    digit_low = value & 0x0F;
    digit_high = value >> 4;
    TRISB &= 0xFC00;

    if (!displayFlag)
        LATB = (LATB & 0xFC00) | 0x200 | display7Scodes[digit_low];
    else
        LATB = (LATB & 0xFC00) | 0x100 | display7Scodes[digit_high];

    displayFlag = !displayFlag;
}

unsigned char toBCD(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}

void configADC(void) {
    TRISBbits.TRISB14 = 1;
    AD1PCFGbits.PCFG14 = 0;

    AD1CON1bits.SSRC = 7;
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16;
    AD1CON2bits.SMPI = N_SAMPLES-1; 
    AD1CHSbits.CH0SA = 14;
    AD1CON1bits.ON = 1;

    IFS1bits.AD1IF = 0;
    IPC6bits.AD1IP = 2;
    IEC1bits.AD1IE = 1;
}

unsigned int convertADC(int *p) {
    int sum = 0, i;
    for( i = 0; i <= N_SAMPLES; i++ )
        sum += p[i<<2];
    return ((sum / N_SAMPLES) * 99 + 511) / 1023;
}

void configT2(void) {
    // 120Hz
    T2CONbits.TCKPS = 7;
    PR2 = 650;
    TMR2 = 0;
    T2CONbits.TON = 1;

    IFS0bits.T2IF = 0;
    IPC2bits.T2IP = 2;
    IEC0bits.T2IE = 1;
}


int main(void) {
    char ch;
    configADC();
    configT2();
    comDrv_config(115200);
    comDrv_flushRx();
    comDrv_flushTx();
    EnableInterrupts();

    while (1) {
        comDrv_getc(&ch);
        if (ch == 'p')
            flag = !flag;
        ch = 0;
    }
}

void _int_(27) isr_adc(void) {
    int *p = (int *)(&ADC1BUF0);
    value2display = convertADC(p);
    IFS1bits.AD1IF = 0;
}

void _int_(8) isr_t2(void) {
    unsigned char value = toBCD(value2display);
    send2displays(value);
    if (++i >= 12) {
        AD1CON1bits.ASAM = 1;
        i = 0;
    }
    if (flag && ++c >= 240) {
        comDrv_putc((value >> 4) + '0');
        comDrv_putc((value & 0x0F) + '0');
        comDrv_putc('\n');
        c = 0;
    }
    IFS0bits.T2IF = 0;
}

void _int_(24) isr_uart1(void) {
    if (IFS0bits.U1TXIF) {
        if (txb.count) {
            U1TXREG = txb.data[txb.head];
            txb.head = (txb.head + 1) & INDEX_MASK;
            txb.count--;
        }
        if (!txb.count)
            DisableUart1TxInterrupt();
        IFS0bits.U1TXIF = 0;
    }
    if (IFS0bits.U1RXIF) {
        rxb.data[rxb.tail] = U1RXREG;
        rxb.tail = (rxb.tail + 1) & INDEX_MASK;
        if (rxb.count < BUF_SIZE)
            rxb.count++;
        else
            rxb.head = (rxb.head + 1) & INDEX_MASK;
        IFS0bits.U1RXIF = 0;
    }
}
