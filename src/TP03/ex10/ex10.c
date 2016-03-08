#include "send2displays.c"

int main(void) {
    unsigned int i = 0;
    unsigned char c = 0;
    while (1) {
        i = 0;
        do {
            delay(10);                  // delay of 10 ms of displays refresh rate
            send2displays(c);
        } while (++i < 20);
        if (++c > 255)
            c = 0;
    }
    return 0;
}
