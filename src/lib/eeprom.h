#ifndef __EEPROM_H__
#define __EEPROM_H__
#include <detpic32.h>

/* EEPROM COMMANDS */
#define WRITE   0x02
#define READ    0x03
#define RDSR    0x05
#define WREN    0x06
/* END EEPROM COMMANDS */

/**********************************************/
/* Configures SPI2                            */
/**********************************************/
void spi2_init(void);

/**********************************************/
/* Configures SPI2 clock                      */
/**********************************************/
void spi2_setClock(unsigned int clock_freq);

/**********************************************/
/* Read status from EEPROM                    */
/**********************************************/
char eeprom_readStatus(void);

/**********************************************/
/* Write status into EEPROM                   */
/**********************************************/
void eeprom_writeStatusCommand(char command);

/**********************************************/
/* Write data into EEPROM                     */
/**********************************************/
void eeprom_writeData(int address, char value);

/**********************************************/
/* Read data from EEPROM                      */
/**********************************************/
char eeprom_readData(int address);

#endif /* __EEPROM_H__ */
