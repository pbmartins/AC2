#include <detpic32.h>

int main(void) {
    TRISE = (TRISE & 0xF00) | 0x0F0;

    while (1)
        LATE = (PORTE & 0x0F0) >> 4;
    
    return 0;
}
