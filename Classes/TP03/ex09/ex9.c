#include "send2displays.c"

int main(void) {
    unsigned char c = 0;
    while (1) {
        send2displays(c);
        //printInt10(c);
        delay(200);
        c++;
    }
    return 0;
}
