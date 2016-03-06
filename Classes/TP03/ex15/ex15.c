#include "send2displays.c"

unsigned char toBcd(unsigned char value){
   return ((value / 10) << 4) + (value % 10);
}

int main(void) {
    unsigned int i = 0, rnd = 0;
    unsigned char c = 0;
    while (1) {
        i = 0;
        if (c == 0 && rnd) {
            do {
                delay(10);                  // delay of 10 ms of displays refresh rate
                if ((i / 50) % 2 == 0)
                    send2displays(toBcd(c));
                else
                    send2displays(60);
            } while(++i < 500);
        } else {
            do {
                delay(10);                  // delay of 10 ms of displays refresh rate
                send2displays(toBcd(c));
            } while (++i < 100);
        }

        if (++c > 59) {
            c = 0;
            rnd = 1;
        }
    }
    return 0;
}
