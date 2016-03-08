#include "send2displays.c"

unsigned char toBcd(unsigned char value){
   return ((value / 10) << 4) + (value % 10);
}

int main(void) {
    unsigned int i = 0;
    unsigned char c = 0;
    while (1) {
        i = 0;
        do {
            delay(10);                  // delay of 10 ms of displays refresh rate
            send2displays(toBcd(c));
        } while (++i < 100);
        if (++c > 59)
            c = 0;
    }
    return 0;
}
