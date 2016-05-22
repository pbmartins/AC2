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

void configT2() {
    // 100Hz
    T2CONbits.TCKPS = 5;
    PR2 = 6249;
    TMR2 = 0; 
    T2CONbits.TON = 1;

    IFS0bits.T2IF = 0;
    IPC2bits.T2IP = 2;
    IEC0bits.T2IE = 1;
}

void configT3(unsigned int duty_cycle) {
    // 50Hz
    T2CONbits.TCKPS = 5;
    PR2 = 12499;
    TMR2 = 0; 
    T2CONbits.TON = 1;

    OC1CONbits.OCM = 6;
    OC1CONbits.OCTSEL =0;
    OC1RS = ((PR2 + 1) * duty_cycle) / 100;
    OC1CONbits.ON = 1;
}


int main(void) {
    TRISE |= 0x0F0;
    configUART(115200);
    configT2();
    configT3(50);
    EnableInterrupts();

    while (1);
}

void _int_(8) isr_T2(void) {
    static unsigned char value = 0, i = 0;
    value = toBCD((PORTE & 0x0F0) >> 4);
    if (++i >= 100) {
        putc(((value >> 4) & 0x0F) + '0');
        putc((value & 0x0F) + '0');
        i = 0;
    }

    IFS0bits.T2IF = 0;
}
