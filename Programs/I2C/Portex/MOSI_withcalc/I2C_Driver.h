#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <stdint.h>

#define STD		0
#define FAST	1

typedef struct
{
    float tr_max;
    float tf_max;
    float fSCL;
    float tSU_DAT_min;
    float tHigh_min;

}i2c_Mode_TypeDef;

//extern int PRESC, SCLDEL, SDADEL, SCLH, SCLL;

/* Prototypes */
void Stdmode(void);

void Fastmode(void);

unsigned int i2c_timingr(uint8_t mode, float tr, float tf);

void I2C_init(uint8_t mode, uint8_t tr, uint8_t tf);			//needs to be in FINAL CODE

void I2C2_init(void);


void i2c_begin(uint8_t slav_add, uint8_t nbytes);
void i2c_stop(void);
void i2c_transmit(uint8_t data);
uint8_t i2c_receive(void);

#endif			//I2C_DRIVER_H

