#include "i2c_timr.h"
#include <math.h>
#include <stdint.h>
#include "stm32f3xx.h"                  // Device header

int PRESC, SCLDEL, SDADEL, SCLH, SCLL;



Mode_TypeDef Mode;


void Stdmode(void)
{
    Mode.tf_max = 300*pow(10,-9);
    Mode.tr_max = 1000*pow(10,-9);
    Mode.fSCL = 100000;
    Mode.tSU_DAT_min = 250*pow(10,-9);
    Mode.tHigh_min = 4*pow(10,-6);
}

void Fastmode(void)
{
    Mode.tf_max = 300*pow(10,-9);
    Mode.tr_max = 300*pow(10,-9);
    Mode.fSCL = 400000;
    Mode.tSU_DAT_min = 100*pow(10,-9);
    Mode.tHigh_min = 0.6*pow(10,-6);
}


void i2c_timingr(int mode, double tr, double tf)
{
		
	
    double tlow_min, taf_min, taf_max, tclk, tpresc, tSCL ;
		
		uint32_t timr;

    double fclk = SystemCoreClock;                  //x MHz (here 8 MHz)
		
		int AF = 0; //(I2C2->CR1 & I2C_CR1_ANFOFF);
    double tDNF = 0; //(I2C2->CR1 & I2C_CR1_DNF);    

		tr = tr*pow(10,-9);
		tf = tf*pow(10,-9);

		if(AF==0)
		{
			taf_min = 0.05*pow(10,-6);
			taf_max = 0.26*pow(10,-6);
		}
		else
		{
			taf_min = 0;
			taf_max = 0;
		}

	
    if(mode==0) Stdmode();
    else Fastmode();

    if(tr>Mode.tr_max) tr = Mode.tr_max;         //if user entered value is more than max possible, set as max possible
    if(tf>Mode.tf_max) tf = Mode.tf_max;

		
		PRESC = 2;
    tclk = 1/fclk;

    tpresc = (PRESC+1)*tclk;

    tSCL = 1/Mode.fSCL;

    SCLDEL = round(((tr+Mode.tSU_DAT_min)/tpresc) - 1);

    SDADEL = round((tf-taf_min-tDNF-(3*tclk))/tpresc);

    if(SDADEL<0) SDADEL=0;


    SCLH = round(((Mode.tHigh_min - taf_min - tDNF - (2*tclk))/tpresc) - 1);

    tlow_min = tSCL - tf - tr - Mode.tHigh_min;

    SCLL = round(((tlow_min - taf_min - tDNF - (2*tclk))/tpresc) - 1);

		
		//timr = (uint32_t) (PRESC<<28 | SCLDEL<<20 | SDADEL<<16 | SCLH<<8 | SCLL<<0);
		
		//return timr;
	
}