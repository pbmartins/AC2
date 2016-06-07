#include <detpic32.h>
#include "../../lib/convert2volt.h"
#include "../../lib/displays.h"
#include "../../lib/delay.h"
#include "../../lib/initadc.h"
#include "../../lib/i2c.h"

#define ADDR_WR         ((SENS_ADDRESS << 1) | I2C_WRITE)
#define ADDR_RD         ((SENS_ADDRESS << 1) | I2C_READ)
#define SENS_ADDRESS    0x4D
#define TC74_CLK_FREQ   100000          // 100 KHz
#define RTR             0               // Read temperature command

volatile unsigned char value2displays;
static unsigned int nConvertions = 8;
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
    TMR3 = 0;                                               // Reset timer T1 count register
    T3CONbits.TON = 1;                                      // Enable timer T1 (must be the last command of the timer configuration sequence)

    IFS0bits.T3IF = 0;                                      // Reset timer T3 interrupt flag
    IPC3bits.T3IP = 2;                                      // Interrupt priority (must be in range [1..6])
    IEC0bits.T3IE = 1;                                      // Enable timer T3 interrupts

    TRISE = TRISE | 0x0F0;                                  // Configure RE4-RE7 as inputs
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

int getTemperature(int *temp) {
    int ack;

    i2c1_start();                       // Send Start event
    ack = i2c1_send(ADDR_WR);           // Send Address + WR (ADDR_WR)
    ack += i2c1_send(RTR);              // Send Command (RTR)
    i2c1_start();                       // Send Start event (again)
    ack += i2c1_send(ADDR_RD);          // Send Address + RD (ADDR_RD)
    if (ack) {                          // Test "ack" variable
        i2c1_stop();                    // send the Stop event
        printStr("\nAn error has ocurred");
        return -1;
    }
    *temp = i2c1_receive(I2C_NACK);      // Receive a value from slave copy
    i2c1_stop();                        // Send Stop event
}

int main(void) {
    int temp;
    configureAll();
    i2c1_init(TC74_CLK_FREQ);
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
            getTemperature(&temp);
            value2displays = temp;
            break;
        }
    }
    return 0;
}

// Interrupt Handler
void _int_(27) isr_adc(void) {
    int *p = (int *)(&ADC1BUF0);
    value2displays = convert2volt(p, nConvertions);
    IFS1bits.AD1IF = 0;                                     // Reset AD1IF
}

void _int_(4) isr_T1(void) {
    AD1CON1bits.ASAM = 1;                                   // Start A/D conversion
    IFS0bits.T1IF = 0;                                      // Reset T1IF flag
}

void _int_(12) isr_T3(void) {
    // Send "value2display" global variable to displays
    send2displays(toBcd(value2displays));
    if (readMode() != 2 || readMode() != 3)
        send2dots(0x0010);
    IFS0bits.T3IF = 0;                                      // Reset T3IF flag
}
