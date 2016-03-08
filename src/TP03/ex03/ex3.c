#include <detpic32.h>

int main(void) {
    TRISB = (TRISB & 0xFFC00);
    LATBbits.LATB9 = 1;
    LATBbits.LATB8 = 0;
    char c;

    while (1) {
        c = getChar();
        switch (c) {
            case 'a': case 'A':
                LATB = (LATB & 0xFF00) | 0x0002;
                break;
            case 'b': case 'B':
                LATB = (LATB & 0xFF00) | 0x0001;
                break;
            case 'c': case 'C':
                LATB = (LATB & 0xFF00) | 0x0040;
                break;
            case 'd': case 'D':
                LATB = (LATB & 0xFF00) | 0x0020;
                break;
            case 'e': case 'E':
                LATB = (LATB & 0xFF00) | 0x0010;
                break;
            case 'f': case 'F':
                LATB = (LATB & 0xFF00) | 0x0004;
                break;
            case 'g': case 'G':
                LATB = (LATB & 0xFF00) | 0x0008;
                break;
            case '.':
                LATB = (LATB & 0xFF00) | 0x0080;
                break;
            default:
                break;
        }
    }
    return 0;
}
