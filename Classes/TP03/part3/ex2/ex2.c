#include "delay.c"

int main(void) {
    int v = 0;
    TRISEbits.TRISE4 = 0;
    while (1) {
        LATEbits.LATE4 = v; //Escreve v no bit 4 do porto E
        delay(1); // Atraso de 1ms
        v ^= 1; // complementa o bit 0 de v
    }
    return 0;
}
