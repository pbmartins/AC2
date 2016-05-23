#include <detpic32.h>

int main(void) {
    unsigned char value;
    TRISE = (TRISE & 0xF00) | 0x0F0;

    while (1) {
        value = (PORTE & 0x0F0) >> 4;
        LATEbits.LATE0 = value >> 3;
        LATEbits.LATE1 = (value >> 2) & 0x01;
        LATEbits.LATE2 = (value >> 1) & 0x01;
        LATEbits.LATE3 = value & 0x01;
    }
    return 0;
}
