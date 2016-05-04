#include <detpic32.h>
#include "../../../lib/eeprom.h"
#define EEPROM_CLOCK 500000
#define WREN 6

void main(void) {
    spi2_init();
    spi2_setClock(EEPROM_CLOCK);
    for(;;) {
        eeprom_writeStatusCommand(WREN);
        printInt10(eeprom_readStatus() & 0x0F);
        putChar('\n');
    }
}
