#include <detpic32.h>
#include "../../lib/i2c.h"
#include "../../lib/delay.h"

#define ADDR_WR         ((SENS_ADDRESS << 1) | I2C_WRITE)
#define ADDR_RD         ((SENS_ADDRESS << 1) | I2C_READ)
#define SENS_ADDRESS    0x4D
#define TC74_CLK_FREQ   100000          // 100 KHz
#define RTR             0               // Read temperature command

int getTemperature(int *temp) {
    int ack;

    i2c1_start();                       // Send Start event
    ack = i2c1_send(ADDR_WR);           // Send Address + WR (ADDR_WR)
    ack += i2c1_send(RTR);              // Send Command (RTR)
    i2c1_start();                       // Send Start event (again)
    ack += i2c1_send(ADDR_RD);          // Send Address + RD (ADDR_RD)
    if (ack) {                          // Test "ack" variable
        i2c1_stop();                    // send the Stop event
        printStr("\nAn error has ocurred");
        return -1;
    }
    *temp = i2c1_receive(I2C_NACK);      // Receive a value from slave copy
    i2c1_stop();                        // Send Stop event
}

int main(void) {
    int temperature;
    i2c1_init(TC74_CLK_FREQ);

    while (1) {
        getTemperature(&temperature);
        printInt10(temperature);
        putChar('\n');
        //delay(250);
    }
    return 0;
}
