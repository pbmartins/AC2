#ifndef I2C_H
#define I2C_H

#include <detpic32.h>

#define I2C_READ    1
#define I2C_WRITE   0
#define I2C_ACK     0
#define I2C_NACK    1

void i2c1_init(unsigned int clock_freq);

void i2c1_start(void);

void i2c1_stop(void);

int i2c1_send(unsigned char value);

char i2c1_receive(char ack_bit);

#endif
