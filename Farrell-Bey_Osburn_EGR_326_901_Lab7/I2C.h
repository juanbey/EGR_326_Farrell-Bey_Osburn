#include "msp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SCL BIT5     // P6.5
#define SDA BIT4     // P6.4



void I2C_init(void);

int I2C_write(int slaveAddr, unsigned char memAddr, unsigned char* data);
int I2C_read(int slaveAddr, unsigned char memAddr, unsigned char* data);

void I2C_print(unsigned char *data);
//int I2C_burstwrite(int slaveAddr, unsigned char* memAddr, unsigned char* data);
//int I2C_burstread(int slaveAddr, unsigned char memAddr, unsigned char* data);