#include "convert2volt.h"

int convert2volt(int *p, int n) {
    int sum = 0;
    unsigned int i;
    for (i = 0; i < n; i++)
        sum += p[i * 4];

    return ((sum / n) * 33 + 511) / 1023;
}

