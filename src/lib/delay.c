#ifndef __DELAY__
#define __DELAY__
#define CALIBRATION_VALUE 9966

void delay(unsigned int n_intervals) {
    volatile unsigned int i;
    for (; n_intervals != 0; n_intervals--)
        for (i = CALIBRATION_VALUE >> 1; i != 0; i--);
}
#endif /* __DELAY__ */
