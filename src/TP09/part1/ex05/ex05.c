#include <detpic32.h>
#include "../../../lib/eeprom.h"
#define EEPROM_CLOCK 500000

void main(void) {
    spi2_init();
    spi2_setClock(EEPROM_CLOCK);
    for(;;) {
        printInt10(eeprom_readStatus() & 0x0F);
        putChar('\n');
    }
}
