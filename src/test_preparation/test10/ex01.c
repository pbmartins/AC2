#include <detpic32.h>
#define N_SAMPLES   15
#define CLOCK_FREQ  500000
#define RDSR        0x05
#define WRITE       0x02
#define READ        0x03
#define WREN        0x06

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
static unsigned char tmpADC = 0, value2display = 0, displayFlag = 0, memEnable = 0;

void configUART(unsigned int baudrate) {
    U1MODEbits.BRGH = 0;
    U1BRG = ((PBCLK + 8 * baudrate) / (16 * baudrate)) - 1;
    U1MODEbits.PDSEL = 2;
    U1MODEbits.STSEL = 0;
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 1;
    U1MODEbits.ON = 1;

    U1STAbits.URXISEL = 0;
    IPC6bits.U1IP = 2;
    IFS0bits.U1TXIF = IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 1;
    IEC0bits.U1EIE = 1;
}

void putc(char byte2send) {
    while (U1STAbits.UTXBF);
    U1TXREG = byte2send;
}

void puts(char *str) {
    while (*str)
        putc(*str++);
}

void configADC(void) {
    TRISBbits.TRISB14 = 1; 
    AD1PCFGbits.PCFG14 = 0;
    AD1CON1bits.SSRC = 7;
    AD1CON1bits.CLRASAM = 1;
    AD1CON3bits.SAMC = 16;
    AD1CON2bits.SMPI = N_SAMPLES - 1;
    AD1CHSbits.CH0SA = 14;
    AD1CON1bits.ON = 1;

    IFS1bits.AD1IF = 0;
    IPC6bits.AD1IP = 2;
    IEC1bits.AD1IE = 1;
}

unsigned int convertADC(int* p) {
    int sum = 0, i;
    for (i = 0; i <= N_SAMPLES; i++)
        sum += p[i<<2];
    return ((sum / N_SAMPLES) * 99 + 511) / 1023;
}

unsigned char toBCD(unsigned char value) {
    return ((value / 10) << 4) + (value % 10);
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

void configT4(void) {
    T4CONbits.TCKPS = 2;
    PR4 = 34482;
    TMR4 = 0;
    T4CONbits.TON = 1;

    IFS0bits.T4IF = 0;
    IPC4bits.T4IP = 2;
    IEC0bits.T4IE = 1;
}

void setPWM(unsigned int duty_cycle) {
    if (duty_cycle > 100)
        duty_cycle = 100;
    OC1RS = ((PR2 + 1) * duty_cycle) / 100;
}


void configOC1() {
    T2CONbits.TCKPS = 5;
    PR2 = 6249;
    TMR2 = 0;
    T2CONbits.TON = 1;

    OC1CONbits.OCM = 6; 
    OC1CONbits.OCTSEL = 0;
    setPWM(0);
    OC1CONbits.ON = 1;
}

void spi2_setClock(unsigned int clock_freq) {
    SPI2BRG = (PBCLK + clock_freq) / (2 * clock_freq) - 1;
}

void spi2_init(void) {
    volatile char trash;
    SPI2CONbits.ON = 0;
    SPI2CONbits.CKP = 0;
    SPI2CONbits.CKE = 1;
    SPI2CONbits.SMP = 0;
    SPI2CONbits.MODE32 = 0;
    SPI2CONbits.MODE16 = 0;
    SPI2CONbits.ENHBUF = 1;
    SPI2CONbits.MSSEN = 1;
    SPI2CONbits.MSTEN = 1;
    while(!SPI2STATbits.SPIRBE)
        trash = SPI2BUF;
    SPI2STATbits.SPIROV = 0;
    SPI2CONbits.ON = 1;
}

char eeprom_readStatus(void) {
    volatile char trash;
    while(!SPI2STATbits.SPIRBE)
        trash = SPI2BUF;
    SPI2STATbits.SPIROV = 0;
    SPI2BUF = RDSR;
    SPI2BUF = 0;
    while(SPI2STATbits.SPIBUSY); 
    trash = SPI2BUF;
    return SPI2BUF;
}

void eeprom_writeStatusCommand(char command) {
    while( eeprom_readStatus() & 0x01 );
    SPI2BUF = command;
    while (SPI2STATbits.SPIBUSY);
}

void eeprom_writeData(int address, char value) {
    address &= 0x1FF;
    while( eeprom_readStatus() & 0x01 );
    eeprom_writeStatusCommand(WREN);
    SPI2BUF = WRITE | ((address & 0x100) >> 5);
    SPI2BUF = address & 0x0FF;
    SPI2BUF = value;
    while (SPI2STATbits.SPIBUSY);
}

char eeprom_readData(int address) {
    volatile char trash;
    while(!SPI2STATbits.SPIRBE)
        trash = SPI2BUF;
    SPI2STATbits.SPIROV = 0;
    address &= 0x1FF;
    while( eeprom_readStatus() & 0x01 );
    SPI2BUF = READ | ((address & 0x100) >> 5);
    SPI2BUF = address & 0x0FF;
    SPI2BUF = 0;
    while (SPI2STATbits.SPIBUSY);
    trash = SPI2BUF;
    trash = SPI2BUF;
    return SPI2BUF;
}

int main(void) {
    configUART(38400);
    configADC();
    configT4();
    configOC1();
    spi2_init();
    spi2_setClock(CLOCK_FREQ);
    eeprom_writeData(0, 0);
    EnableInterrupts();

    while (1);
}

void _int_(27) isr_adc(void) {
    int address, *p = (int *)(&ADC1BUF0);
    tmpADC = convertADC(p);
    if (memEnable) {
        if ((address = 2 + eeprom_readData(0)) < 256) {
            eeprom_writeData(address, tmpADC);
            eeprom_writeData(0, address - 1);
        } else {
            eeprom_writeData(2, tmpADC);
            eeprom_writeData(0, 1);
        }
    }

    IFS1bits.AD1IF = 0;
}

void _int_(16) isr_t4(void) {
    send2displays(value2display);
    IFS0bits.T4IF = 0;
}

void _int_(24) isr_uart1(void) {
    unsigned char value, memVal;
    int address, i;
    if (IFS0bits.U1EIF) {
        if (U1STAbits.OERR)
            U1STAbits.OERR = 0;
        else
            value = U1RXREG;
        IFS0bits.U1EIF = 0;
    }
    if (IFS0bits.U1RXIF) {
        value = U1RXREG;
        if (value == 'A')
            AD1CON1bits.ASAM = 1;
        else if (value == 'D')
            value2display = toBCD(tmpADC);
        else if (value == 'E') {
            puts("\nADC value: ");
            putc(((toBCD(tmpADC) & 0x0F0) >> 4) + '0');
            putc((toBCD(tmpADC) & 0x0F) + '0');
        } else if (value == 'P')
            setPWM(tmpADC);
        else if (value == 'M')
            memEnable = !memEnable;
        else if (value == 'L') {
            address = eeprom_readData(0);
            puts("\nValues stored in memory: ");
            for (i = 0; i < address; i++) {
                putc('\n')
                memVal = toBCD(eeprom_readData(2 + i));
                putc(((memVal & 0x0F0) >> 4) + '0');
                putc((memVal & 0x0F) + '0');
            }
        }
        IFS0bits.U1RXIF = 0;
    }
}
