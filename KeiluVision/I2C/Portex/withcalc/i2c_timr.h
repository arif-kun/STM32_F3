#ifndef I2C_TIMR_H
#define I2C_TIMR_H

typedef struct
{
    double tr_max;
    double tf_max;
    double fSCL;
    double tSU_DAT_min;
    double tHigh_min;


}Mode_TypeDef;

extern int PRESC, SCLDEL, SDADEL, SCLH, SCLL;


void Stdmode(void);

void Fastmode(void);

void i2c_timingr(int mode, double tr, double tf);


#endif