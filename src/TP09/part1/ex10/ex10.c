#include <detpic32.h>
#include "../../../lib/eeprom.h"
#define EEPROM_CLOCK 500000

int main(void) {
    char ch;
    int addr, val;
    spi2_init();
    spi2_setClock(EEPROM_CLOCK);
    while (1) {
        printStr("Option (R|W): ");
        ch = getChar();
        putChar(ch);
        putChar('\n');
        if (ch == 'R') {
            printStr("Address: ");
            addr = readInt10();
            putChar('\n');
            printInt(eeprom_readData(addr), 0x00080010);
        } else if (ch == 'W') {
            printStr("Address: ");
            addr = readInt10();
            putChar('\n');
            printStr("Value: ");
            val = getChar();
            putChar(val);
            putChar('\n');
            eeprom_writeData(addr, val);
        }
        putChar('\n');
    }
    return 0;
}
