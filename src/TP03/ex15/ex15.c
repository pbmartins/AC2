#include "../../lib/send2displays.c"

int main(void) {
    unsigned int i = 0, rnd = 0, value;
    unsigned char c = 0;
    while (1) {
        i = 0;
        value = toBcd(c);
        if (c == 0 && rnd) {
            do {
                delay(10);                  // delay of 10 ms of displays refresh rate
                if ((i / 50) % 2 == 0)
                    send2displays(value);
                else
                    send2displays(60);
            } while(++i < 500);
        } else {
            do {
                delay(10);                  // delay of 10 ms of displays refresh rate
                send2displays(value);
                if (c & 1)
                    send2dots(0x0001);
                else
                    send2dots(0x0010);
            } while (++i < 100);
        }

        if (++c > 59) {
            c = 0;
            rnd = 1;
        }
    }
    return 0;
}
