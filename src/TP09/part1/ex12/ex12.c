#include <detpic32.h>
#include "../../../lib/eeprom.h"
#define EEPROM_CLOCK 500000

int main(void) {
    char ch;
    int addr, val, i;
    spi2_init();
    spi2_setClock(EEPROM_CLOCK);
    while (1) {
        i = 0;
        printStr("Option (R|W): ");
        ch = getChar();
        putChar(ch);
        putChar('\n');
        if (ch == 'R') {
            printStr("Initial address: ");
            addr = readInt10();
            putChar('\n');
            while (i++ < 16) {
                printInt(eeprom_readData(addr + i), 0x00080010);
                putChar(' ');
            }
        } else if (ch == 'W') {
            printStr("Initial address: ");
            addr = readInt10();
            putChar('\n');
            printStr("Inital value: ");
            val = getChar();
            putChar(val);
            putChar('\n');
            while (i++ < 16)
                eeprom_writeData(addr + i, val++);
        }
        putChar('\n');
    }
    return 0;
}
