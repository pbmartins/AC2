#include <detpic32.h>
#include "../../../lib/convert2volt.h"
#include "../../../lib/displays.h"
#include "../../../lib/delay.h"
#include "../../../lib/initadc.h"
#include "../../../lib/configuart.h"
#include "../../../lib/eeprom.h"
#define EEPROM_CLOCK 500000

volatile unsigned char value2displays, voltMin = 33, voltMax = 0;
static unsigned int nConvertions = 8, counter = 0,
    beginStoring = 0, eeprom_cnt = 0;
const static unsigned char pwmValues[] = {3, 15, 40, 90};

void configureAll(void) {
    initADC(nConvertions);                                  // Configure ADC with nConvertions = 8
    IFS1bits.AD1IF = 0;                                     // Rest AD1IF
    IPC6bits.AD1IP = 6;                                     // Configure interrupt priority
    IEC1bits.AD1IE = 1;                                     // Enable AD1 Interrupt

    // Configure Timer T1 (4 Hz, interrupts disabled)
    T1CONbits.TCKPS = 7;                                    // 1:256 prescaler (i.e fin = 78125 Hz)
    PR1 = 19530;                                            // Fout = 20MHz / (256 * (19530 + 1)) = 4 Hz
    TMR1 = 0;                                               // Reset timer T1 count register
    T1CONbits.TON = 1;                                      // Enable timer T1 (must be the last command of the timer configuration sequence)

    IFS0bits.T1IF = 0;                                      // Reset timer T1 interrupt flag
    IPC1bits.T1IP = 2;                                      // Interrupt priority (must be in range [1..6])
    IEC0bits.T1IE = 1;                                      // Enable timer T1 interrupts

    // Configure Timer T3 (100 Hz, interrupts disabled)
    T3CONbits.TCKPS = 5;                                    // 1:32 prescaler (i.e fin = 625000 Hz)
    PR3 = 6250;                                             // Fout = 20MHz / (32 * (6250 + 1)) = 100 Hz
    TMR3 = 0;                                               // Reset timer T3 count register
    T3CONbits.TON = 1;                                      // Enable timer T3 (must be the last command of the timer configuration sequence)

    IFS0bits.T3IF = 0;                                      // Reset timer T3 interrupt flag
    IPC3bits.T3IP = 2;                                      // Interrupt priority (must be in range [1..6])
    IEC0bits.T3IE = 1;                                      // Enable timer T3 interrupts

    // Configure UART1
    configUART(115200, 'N', 1);
    enableUARTInterrupts(0);

    TRISE = TRISE | 0x0F0;                                  // Configure RE4-RE7 as inputs

    // Configure EEPROM
    spi2_init();
    spi2_setClock(EEPROM_CLOCK);
}

int readMode(void) {
    return ((PORTE >> 4) & 0x03);
}

int readLEDValues(void) {
    return ((PORTE >> 6) & 0x03);
}

void setPWM(unsigned int dutyCycle) {
    // dutyCycle must be in the range [0, 100]
    if (dutyCycle < 0 || dutyCycle > 100)
        return;

    OC1CONbits.OCM = 6;                                     // PWM mode on OC1; fault pin disabled 
    OC1CONbits.OCTSEL = 1;                                  // Use timer T3 as the time base for PWM generation
    OC1RS = ((PR3 + 1) * dutyCycle) / 100;                  // Evaluate OC1RS as a function of "dutyCycle"
    OC1CONbits.ON = 1;                                      // Enable OC1 module
}

void putc(char byte2send) {
    while (U1STAbits.UTXBF == 1);
    U1TXREG = byte2send;
}

void puts(char *str) {
    char c;
    while ((c = *str++) != 0)
        putc(c);
}

int main(void) {
    configureAll();
    EnableInterrupts();                                     // Global Interrupt Enable

    while (1) {
       switch (readMode()) {
        case 0:                                             // Measure input voltage
            IEC0bits.T1IE = 1;                              // Enable timer T1 interrupts
            setPWM(0);                                      // Turn off LED
            break;
        case 1:                                             // Freeze
            IEC0bits.T1IE = 0;                              // Disable timer T1 interrupts
            setPWM(100);                                    // Turn on LED with max brightness
            break;

        case 2:                                             // LED brigthness control
            IEC0bits.T1IE = 0;                              // Disable timer T1 interrupts
            value2displays = pwmValues[readLEDValues()];
            setPWM(value2displays);
            break;

        case 3:
            break;
        }
    }
    return 0;
}

// Interrupt Handler
// ADC
void _int_(27) isr_adc(void) {
    int *p = (int *)(&ADC1BUF0);
    value2displays = convert2volt(p, nConvertions);
    // Update minumum and maximum volt values
    if (value2displays < voltMin)
        voltMin = value2displays;
    if (value2displays > voltMax)
        voltMax = value2displays;
    IFS1bits.AD1IF = 0;                                     // Reset AD1IF
}

// Timer 1
void _int_(4) isr_T1(void) {
    char cnt;

    AD1CON1bits.ASAM = 1;                                   // Start A/D conversion
    IFS0bits.T1IF = 0;                                      // Reset T1IF flag

    if (beginStoring) {
        cnt = eeprom_readData(0);
        if (eeprom_cnt++ >= 45 && cnt < 64) {
            eeprom_cnt = 0;
            eeprom_writeData(cnt + 2, value2displays);
            eeprom_writeData(0, ++cnt);
        }
    }
}

// Timer 3
void _int_(12) isr_T3(void) {
    unsigned char val = toBcd(value2displays);
    // Send "value2display" global variable to displays
    send2displays(val);
    if (readMode() != 2)
        send2dots(0x0010);
    // Send "value2displays" global variable to PC through UART1
    if (counter++ >= 99) {
        putc(((val & 0x0F0) >> 4) + 48);
        putc('.');
        putc((val & 0x0F) + 48);
        putc('\n');
        counter = 0;
    }
    IFS0bits.T3IF = 0;                                      // Reset T3IF flag
}



// UART1
void _int_(24) isr_uart1(void) {
    unsigned char valMin = toBcd(voltMin), valMax = toBcd(voltMax), reg, cnt, i;
    if (IFS0bits.U1EIF) {
        if (U1STAbits.OERR)
            U1STAbits.OERR = 0;
        else
            reg = U1RXREG;
        IFS0bits.U1EIF = 0;
    }
    reg = U1RXREG;
    if (IFS0bits.U1RXIF) {
        if(reg == 'L') {
            beginStoring = 1;
            putc('\n');
            putc(((valMin & 0x0F0) >> 4) + 48);
            putc('.');
            putc((valMin & 0x0F) + 48);
            putc('\n');
            putc(((valMax & 0x0F0) >> 4) + 48);
            putc('.');
            putc((valMax & 0x0F) + 48);
            putc('\n');
            putc('\n');
        } else if (reg == 'R')
            eeprom_writeData(0, 0);
        else if (reg == 'S') {
            putc('\n');
            cnt = eeprom_readData(0);
            i = 0;
            while (i < cnt) {
                putc(eeprom_readData(2 + i++));
                putc(' ');
            }
            putc('\n');
        }
    }
    IFS0bits.U1RXIF = 0;                                    // Clear UART1 rx interrupt flag
}
