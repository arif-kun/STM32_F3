#include "stm32f3xx.h"                  // Device header
#include <stdint.h>
#include <math.h>
#include "PC_Comm.h"
#include <stdio.h>
//#include "i2c_timr.h"

void I2C2_init();
void i2c_write(uint8_t regadd, uint8_t data);
uint8_t i2c_read(uint8_t regadd, uint8_t data);
void delayms(int count);


int PRESC, SCLDEL, SDADEL, SCLH, SCLL;
uint32_t timr;
double tclk;


typedef struct
{
    double tr_max;
    double tf_max;
    double fSCL;
    double tSU_DAT_min;
    double tHigh_min;


}Mode_TypeDef;

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
		
	
    double tlow_min, taf_min, taf_max, tpresc, tSCL ;
		
		

    double fclk = SystemCoreClock/1000;                  //x MHz (here 8 MHz)
		
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

		
		PRESC = 1;
    tclk = 1/(fclk*pow(10,3));

    tpresc = (PRESC+1)*tclk;

    tSCL = 1/Mode.fSCL;

    SCLDEL = ceil(((tr+Mode.tSU_DAT_min)/tpresc) - 1);

    //SDADEL = round((tf-taf_min-tDNF-(3*tclk))/tpresc);

    //if(SDADEL<0) SDADEL=0;
		SDADEL = 0;

    SCLH = ceil(((Mode.tHigh_min - taf_min - tDNF - (2*tclk))/tpresc) - 1);

    tlow_min = tSCL - tf - tr - Mode.tHigh_min;

    SCLL = ceil(((tlow_min - taf_min - tDNF - (2*tclk))/tpresc) - 1);

		
		timr = (unsigned int) (PRESC<<28 | SCLDEL<<20 | SDADEL<<16 | SCLH<<8 | SCLL<<0);
		
		//return timr;
	
}


int main()
{
	//uint8_t regadd = 0x00;
	uint8_t data = 0x00;
	I2C2_init();
	PC_Comm_init(115200);
	
	i2c_write(0x0A, 0x98);
	
	i2c_write(0x00,0x00);
	
	//i2c_timingr(0,1000,100);
	
	
	while(1)
	{
		i2c_write(0x09,data++);
		printf("\r\n\r\ntimr = 0x %X",timr);
		delayms(100);
	}
	
	/*
	while(1)
	{
		
	}
	*/
	
}


void I2C2_init()
{
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	GPIOA->MODER &=~(3<<18 | 3<<20);
	GPIOA->MODER |= 2<<18 | 2<<20;
	
	GPIOA->AFR[1] &=~(0XF<<4 | 0xF<<8);			//PA9 and PA10 AFs
	GPIOA->AFR[1] |= (4<<4 | 4<<8);
	
	GPIOA->OTYPER &=~(1<<9 | 1<<10);
	GPIOA->OTYPER |= 1<<9 | 1<<10;				//open drain
	
	I2C2->CR1 &=~(I2C_CR1_PE);					//disable I2C2 peripheral driver
	
	
	I2C2->TIMINGR &=~(0xFFFFFFFF);
	i2c_timingr(0,1000,100);
	//I2C2->TIMINGR = 1<<28 | 4<<20 | 0<<16 | 14<<8 | 17<<0;

	I2C2->TIMINGR = (unsigned int) (PRESC<<28 | SCLDEL<<20 | SDADEL<<16 | SCLH<<8 | SCLL<<0);
	
	//I2C2->TIMINGR = 0x2030090B;
	
	I2C2->CR2 |= 0x40;			//slave address
	
	I2C2->CR2 &=~(I2C_CR2_NBYTES);
	
	I2C2->CR2 |= 2<<I2C_CR2_NBYTES_Pos;			//number of bytes to transfer
	
	I2C2->CR2 &=~(I2C_CR2_RD_WRN);					//write mode
	
	I2C2->CR1 |= I2C_CR1_PE;							//enable I2C2 peripheral driver
	
}


void i2c_write(uint8_t regadd, uint8_t data)
{
	//I2C2->CR2 |= 0x40;
	
	I2C2->CR2 |= I2C_CR2_START;			//generate start bit
	
	while(!((I2C2->ISR & I2C_ISR_TXE)));
	I2C2->TXDR = regadd;
	
	while(!((I2C2->ISR & I2C_ISR_TXE)));

	I2C2->TXDR = data;
	
	while(!((I2C2->ISR & I2C_ISR_TC)));	
	
	I2C2->CR2 |= I2C_CR2_STOP;				//generate stop
	
	//I2C2->CR1 &=~(I2C_CR1_PE);		
	
}


uint8_t i2c_read(uint8_t regadd, uint8_t data)
{
	volatile uint8_t reading;
	while(I2C2->ISR & I2C_ISR_BUSY);			//wait if bus is busy

	
	I2C2->CR2 |= I2C_CR2_START;			//generate start bit
	
	while(!((I2C2->ISR & I2C_ISR_TXE)));
	I2C2->TXDR = regadd;
	
	while(!(I2C2->ISR & I2C_ISR_RXNE));
	reading = I2C2->RXDR;
	
	I2C2->CR2 |= I2C_CR2_STOP;
	
	return reading;
	
	
	
}




void delayms(int count)
{
	int i;
	SysTick->LOAD = SystemCoreClock/1000 - 1;		//8000 cycles in 1 ms
	SysTick->VAL = 0;					//reset current value to 0
	SysTick->CTRL = 5;				//clksource = internal. counter enabled. interrupt disabled
	
	for(i=0; i<count; i++)			//repeat 1ms cycles count number of times
	{
		while((SysTick->CTRL & 0x10000)==0){}			//do nothing till Countflag becomes 1
	}
	
	SysTick->CTRL=0;		//disable counter
	
}