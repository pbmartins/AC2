#include "i2c.h"

void i2c1_init(unsigned int clock_freq) {
    I2C1BRG = (PBCLK + clock_freq) / (2 * clock_freq) - 1;
    I2C1CONbits.ON = 1;
}

void i2c1_start(void) {
    I2C1CONbits.SEN = 1;                // Activate Start event
    while (I2C1CONbits.SEN);            // Wait for completion of the Start event
}

void i2c1_stop(void) {
    while (I2C1CON & 0x1F);             /* Wait until the lower 5 bits 
                                           of I2CxCON are all 0 */
    I2C1CONbits.PEN = 1;                // Activate Stop event
    while (I2C1CONbits.PEN);            // Wait for completion of the Stop event
}

int i2c1_send(unsigned char value) {
    I2C1TRN = value;                    // Copy "value" to I2C1TRN register
    while (I2C1STATbits.TRSTAT);        /* Wait while master transmission is in
                                           progress (8 bits + ACK\) */
    return I2C1STATbits.ACKSTAT;        // Return acknowledge status bit
}

char i2c1_receive(char ack_bit) {
    while (I2C1CON & 0x1F);             /* Wait until the lower 5 bits of I2C1CON 
                                           are all 0 (the lower 5 bits of I2C1CON 
                                           must be 0 before attempting 
                                           to set the RCEN bit) */
    I2C1CONbits.RCEN = 1;               // Activate RCEN bit
    while (!I2C1STATbits.RBF);          // Wait while byte not received

    // Send ACK / NACK bit
    if (ack_bit != 0 && ack_bit != 1)
        return 0; 
    I2C1CONbits.ACKDT = ack_bit;
    
    I2C1CONbits.ACKEN = 1;              // Start Acknowledge sequence
    while (I2C1CONbits.ACKEN);          // Wait for completion of Acknowledge sequence
    return I2C1RCV;                     // Return received value (I2C1RCV)
}
