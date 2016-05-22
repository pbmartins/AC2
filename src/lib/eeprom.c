#include "eeprom.h"

/**********************************************/
/* Configures SPI2                            */
/**********************************************/
void spi2_init(void) {
    volatile char trash;
    SPI2CONbits.ON = 0;                         // Disable SPI2 module
    SPI2CONbits.CKP = 0;                        /* Configure clock idle state
                                                    as logic level 0 */
    SPI2CONbits.CKE = 1;                        /* Configure the clock active
                                                    transition: from active
                                                    state to idle state */
    SPI2CONbits.SMP = 0;                        /* Configure SPI data input 
                                                    sample phase bit (middle of
                                                    data output time) */
    SPI2CONbits.MODE32 = 0;                     // Configure word length (8 bits)
    SPI2CONbits.MODE16 = 0;
    SPI2CONbits.ENHBUF = 1;                     /* Enable Enhanced buffer mode 
                                                    (this allows the usage 
                                                    of FIFOs RX,TX) */
    SPI2CONbits.MSSEN = 1;                      /* Enable slave select support
                                                    (Master Mode Slave Select) */
    SPI2CONbits.MSTEN = 1;                      // Enable master mode

    while(SPI2STATbits.SPIRBE == 0)             // Clear RX FIFO
        trash = SPI2BUF;
    
    SPI2STATbits.SPIROV = 0;                    // Clear overflow error flag
    SPI2CONbits.ON = 1;                         // Enable SPI2 module
}

/**********************************************/
/* Configures SPI2 clock                      */
/**********************************************/
void spi2_setClock(unsigned int clock_freq) {
    // Write SPI2BRG register
    SPI2BRG = (PBCLK + clock_freq) / (2 * clock_freq) - 1;
}

/**********************************************/
/* Read status from EEPROM                    */
/**********************************************/
char eeprom_readStatus(void) {
    volatile char trash;

    while (!SPI2STATbits.SPIRBE)                // Clear RX FIFO
        trash = SPI2BUF;
    SPI2STATbits.SPIROV = 0;                    // Clear overflow error flag bit

    SPI2BUF = RDSR;                             // Send RDSR command
    SPI2BUF = 0;                                /* Send anything so that EEPROM
                                                    clocks data into SO */
    while(SPI2STATbits.SPIBUSY);                /* wait while SPI module
                                                    is working */
    trash = SPI2BUF;                            /* First char received is garbage
                                                    (received while sending command) */
    return SPI2BUF;                             /* Second received character
                                                    is the STATUS value */
}

/**********************************************/
/* Write status into EEPROM                   */
/**********************************************/
void eeprom_writeStatusCommand(char command) {
    while( eeprom_readStatus() & 0x01 );        // Wait while WIP is true
                                                // (write in progress)
    SPI2BUF = command;                          /* Copy "command" value
                                                    to SPI2BUF (TX FIFO) */
    while (SPI2STATbits.SPIBUSY);               /* Wait while SPI module
                                                    is working (SPIBUSY set) */
}

/**********************************************/
/* Write data into EEPROM                     */
/**********************************************/
void eeprom_writeData(int address, char value) {
    address &= 0x01FF;                          /* Apply a mask to limit address
                                                    to 9 bits */
    while (eeprom_readStatus() & 0x01);         /* Read STATUS and wait while WIP
                                                    is true (write in progress) */
    eeprom_writeStatusCommand(WREN);            /* Enable write operations
                                                    (activate WEL bit in STATUS
                                                    register) */
    SPI2BUF = WRITE | ((address & 0x100) >> 5); /* Copy WRITE command and A8
                                                    address bit to the TX FIFO */
    SPI2BUF = address & 0x0FF;                  /* Copy address (8 LSBits)
                                                    to the TX FIFO */
    SPI2BUF = value;                            // Copy "value" to the TX FIFO
    while (SPI2STATbits.SPIBUSY);               /* Wait while SPI module
                                                    is working (SPIBUSY set) */
}

/**********************************************/
/* Read data from EEPROM                      */
/**********************************************/
char eeprom_readData(int address) {
    volatile char trash;

    while (!SPI2STATbits.SPIRBE)                // Clear RX FIFO
        trash = SPI2BUF;
    SPI2STATbits.SPIROV = 0;                    // Clear overflow error flag bit
    address &= 0x01FF;                          /* Apply a mask to limit address
                                                    to 9 bits */
    while (eeprom_readStatus() & 0x01);         /* Read STATUS and wait while WIP
                                                    is true (write in progress) */
    SPI2BUF = READ | ((address & 0x100) >> 5);  /* Copy WRITE command and A8
                                                    address bit to the TX FIFO */
    SPI2BUF = address & 0x0FF;                  /* Copy address (8 LSBits)
                                                    to the TX FIFO */
    SPI2BUF = 0;                                // Copy any value to the TX FIFO
    while (SPI2STATbits.SPIBUSY);               /* Wait while SPI module
                                                    is working (SPIBUSY set) */
    trash = SPI2BUF;                            // Read and discard 2 characters
    trash = SPI2BUF;                            // from RX FIFO
    return SPI2BUF;                             /* Read RX FIFO and return 
                                                    the corresponding value */
}
