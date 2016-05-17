#include <detpic32.h>
#include "../../lib/eeprom.h"
#define ADC_NSAMPLES    15
#define EEPROM_CLOCK    500000

volatile unsigned char value2display = 0, tmp2display = 0, 
         tmp = 0, displayFlag = 0, enableEEPROM = 0, valEEPROM = 0;
static int addr = 0;
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

/* UART */
void configUART(void) {
    // 384000 bps, 8 data bits, odd parity, 1 stop bit
    unsigned int baudrate = 38400;
    U1MODEbits.BRGH = 0;
    //U1BRG = ((PBCLK + 8 * baudrate) / (16 * baudrate)) - 1;
    U1BRG = PBCLK / (16 * baudrate) - 1;

    U1MODEbits.PDSEL = 2;
    U1MODEbits.STSEL = 0;

    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 1;

    U1MODEbits.ON = 1;

    // Config interruptions
    U1STAbits.URXISEL = 0;

    IPC6bits.U1IP = 2;

    IFS0bits.U1RXIF = 0;
    IFS0bits.U1TXIF = 0;
    IEC0bits.U1RXIE = 1;
    IEC0bits.U1TXIE = 0;
}

void putc(char byte2send) {
    while (U1STAbits.UTXBF);
    U1TXREG = byte2send;
}

void puts(char *str) {
    while (*str)
        putc(*str++);
}

char getc(void) {
    if (U1STAbits.OERR)
        U1STAbits.OERR = 0;
    while (!U1STAbits.URXDA);
    return U1RXREG;
}

/* ADC */
void configADC(void) {
    TRISBbits.TRISB14 = 1; // RB14 digital output disconnected 
    AD1PCFGbits.PCFG14 = 0; // RB14 configured as analog input (AN4)

    AD1CON1bits.SSRC = 7;
    AD1CON1bits.CLRASAM = 1; // Stop conversions when the 1st A/D converter
    AD1CON3bits.SAMC = 16; // Sample time is 16 TAD (TAD = 100 ns)
    AD1CON2bits.SMPI = ADC_NSAMPLES - 1; // Interrupt is generated after XX samples
    AD1CHSbits.CH0SA = 14; 
    AD1CON1bits.ON = 1;

    // Config interrupts
    IPC6bits.AD1IP = 2;
    IFS1bits.AD1IF = 0;
    IEC1bits.AD1IE = 1;
}

int convertADC(int *p) {
    int sum = 0;
    unsigned int i;
    for (i = 0; i < ADC_NSAMPLES; i++)
        sum += p[i<<2];

    return ((sum / ADC_NSAMPLES) * 99 + 511) / 1023;
}

/* Displays */
void configDisplays(void) {
    TRISB = TRISB & 0xFC00;

    // Config T4 (145 Hz)
    T4CONbits.TCKPS = 2; // pre-scaler = 4
    PR4 = 34482;
    TMR4 = 0; 
    T4CONbits.TON = 1;

    IFS0bits.T4IF = 0; 
    IPC4bits.T4IP = 2; 
    IEC0bits.T4IE = 1;
}

unsigned char toBcd(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
}


void send2displays(unsigned char value) {
    unsigned char digit_low, digit_high;
    digit_low = value & 0x0F;
    digit_high = value >> 4;

    if (!displayFlag)
        LATB = (LATB & 0xFC00) | 0x0200 | display7Scodes[digit_low];
    else
        LATB = (LATB & 0xFC00) | 0x0100 | display7Scodes[digit_high];

    displayFlag = !displayFlag;
}

/* Timer 2 */
void setPWM(unsigned int duty_cycle) {
    OC1RS = ((PR2 + 1) * duty_cycle) / 100;
}

void configT2(void) {
    // 100 Hz
    T2CONbits.TCKPS = 5; 
    PR2 = 6249;
    TMR2 = 0;
    T2CONbits.TON = 1;

    OC1CONbits.OCM = 6; // PWM mode on OCx; fault pin disabled 
    OC1CONbits.OCTSEL =0;// Use timer T2 as the time base for PWM generation 
    setPWM(0);
    OC1CONbits.ON = 1;
}

/* Main */

int main(void) {
    configUART();
    configADC();
    configDisplays();
    configT2();
    spi2_init();
    spi2_setClock(EEPROM_CLOCK);
    EnableInterrupts();

    while (1);
    return 0;
}

void _int_(24) isr_uart1(void) {
    char val;
    unsigned char readValue;
    int i = 0;
    if (IFS0bits.U1RXIF) {
        val = U1RXREG;
        if (val == 'A')
            AD1CON1bits.ASAM = 1;
        else if (val == 'D')
            value2display = tmp2display;
        else if (val == 'E') {
            putc('\n');
            putc((tmp2display >> 4) + '0');
            putc((tmp2display & 0x0F) + '0');
        } else if (val == 'P')
            setPWM(tmp);
        else if (val == 'M')
            enableEEPROM = !enableEEPROM;
        else if (val == 'L') {
            puts("Values in memory:\n");
            while (i < addr) {
                readValue = toBcd(eeprom_readData(i++ + 2));
                putc((readValue >> 4) + '0');
                putc((readValue & 0x0F) + '0');
                putc('\n');
            }
        }
        IFS0bits.U1RXIF = 0;
    }
}

void _int_(27) isr_adc(void) {
    int *p = (int *)(&ADC1BUF0);
    tmp = convertADC(p);
    tmp2display = toBcd(tmp);
    IFS1bits.AD1IF = 0;

    if (enableEEPROM) {
        eeprom_writeData(2 + addr, tmp);
        eeprom_writeData(0, addr);
        addr = (addr + 1) % 255;
    }
}

void _int_(16) isr_T4(void) {
    send2displays(value2display);
    IFS0bits.T4IF = 0;
}
