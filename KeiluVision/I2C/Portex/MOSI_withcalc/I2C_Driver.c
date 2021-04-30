#include "stm32f3xx.h"                  // Device header
#include <stdint.h>
#include <tgmath.h>
#include "I2C_Driver.h"
#include "timer.h"

i2c_Mode_TypeDef Mode;


void I2C2_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	GPIOA->MODER &=~(3<<18 | 3<<20);
	GPIOA->MODER |= 2<<18 | 2<<20;
	
	GPIOA->AFR[1] &=~(0XF<<4 | 0xF<<8);			//PA9 and PA10 AFs
	GPIOA->AFR[1] |= (4<<4 | 4<<8);
	
	GPIOA->OTYPER &=~(1<<9 | 1<<10);
	GPIOA->OTYPER |= 1<<9 | 1<<10;				//open drain config
	
	I2C2->CR1 &=~(I2C_CR1_PE);					  //disable I2C2 peripheral driver
	
	I2C2->TIMINGR = 0x00300208;
	
	//I2C2->CR2 &=~(I2C_CR2_NBYTES);
	
	I2C2->CR1 |= I2C_CR1_PE;
		
}



void i2c_transmit(uint8_t data)							
{
	I2C2->TXDR = data;
	while(!((I2C2->ISR & I2C_ISR_TXE)));
	
}


uint8_t i2c_receive(void)
{
	volatile uint8_t read_buffer;
	
	while(!(I2C2->ISR & I2C_ISR_RXNE));
	read_buffer = (uint8_t) I2C2->RXDR;
	
	return read_buffer;
	
}

void i2c_begin(uint8_t slav_add, uint8_t nbytes)
{
	uint8_t rdwr;
	rdwr = slav_add & 0x1;
	I2C2->CR2 = (I2C2->CR2 & ~(0xFF) ) | slav_add;
	I2C2->CR2 = (I2C2->CR2 & ~(0xFF<<16)) | (nbytes<<16);
	
	switch(rdwr)			//check LSB for read/write operation
	{
		case 0 : I2C2->CR2 &=~(I2C_CR2_RD_WRN);			//if LSB is 0, write operation
						 break;
		
		case 1 : I2C2->CR2 |= I2C_CR2_RD_WRN;				//if LSB is 1, read operation
						 break;
		
		default: I2C2->CR2 |=(I2C_CR2_RD_WRN);
						 break;
	}
	
	
	I2C2->CR2 |= I2C_CR2_START;			//generate start bit
	while(I2C2->CR2 & I2C_CR2_START);			//check if the slave address sequence has been transmitted
}



void i2c_stop(void)
{
	I2C2->CR2 |= I2C_CR2_STOP;				//generate stop bit
	
	//while(I2C2->CR2 & I2C_CR2_STOP);		//check if stop condition is generated
	
}
	


/*
unsigned int i2c_timingr(uint8_t mode, float tr, float tf)
{
	int PRESC, SCLDEL, SDADEL, SCLH, SCLL, AF;
	uint32_t timr;
	float tclk,fclk, tlow_min, taf_min, taf_max, tpresc, tSCL,tDNF ;
		
	PRESC = 0;
	
	fclk = SystemCoreClock/1000;                  //x MHz (here 8 MHz)
	
	I2C2->CR1 &=~(I2C_CR1_ANFOFF);		
	I2C2->CR1 &=~(0xF<<I2C_CR1_DNF_Pos);
	
	
	AF = 0;//(I2C2->CR1 & I2C_CR1_ANFOFF);
	tDNF = 0;//(I2C2->CR1 & I2C_CR1_DNF);    

	tr = tr*powf(10,-9);
	tf = tf*powf(10,-9);

	if(AF==0)
	{
		taf_min = 0.05*powf(10,-6);
		taf_max = 0.26*powf(10,-6);
	}
	else
	{
		taf_min = 0;
		taf_max = 0;
	}


	if(mode==0 | mode==STD){
		Mode.tf_max = 300*powf(10,-9);
    Mode.tr_max = 1000*powf(10,-9);
    Mode.fSCL = 100000;
    Mode.tSU_DAT_min = 250*powf(10,-9);
    Mode.tHigh_min = 4*powf(10,-6);
	}
	
	else{
		Mode.tf_max = 300*powf(10,-9);
    Mode.tr_max = 300*powf(10,-9);
    Mode.fSCL = 400000;
    Mode.tSU_DAT_min = 100*powf(10,-9);
    Mode.tHigh_min = 0.6*powf(10,-6);
		
	}

	//if user entered value is more than max possible, set as max possible
	if(tr>Mode.tr_max) tr = Mode.tr_max;         
	if(tf>Mode.tf_max) tf = Mode.tf_max;

	
	tclk = 1/(fclk*powf(10,3));

	tpresc = (PRESC+1)*tclk;

	tSCL = 1/Mode.fSCL;

	SCLDEL = ceilf(((tr+Mode.tSU_DAT_min)/tpresc) - 1);		//SCLDEL equation

	SDADEL = floorf((tf-taf_min-tDNF-(3*tclk))/tpresc);		//SDADEL equation

	if(SDADEL<0) SDADEL=0;
	//SDADEL = 0;

	SCLH = ceilf(((Mode.tHigh_min - taf_min - tDNF - (2*tclk))/tpresc) - 1);		//SCLH equation

	tlow_min = tSCL - tf - tr - Mode.tHigh_min;			//calculate this to find SCLL below

	SCLL = ceilf(((tlow_min - taf_min - tDNF - (2*tclk))/tpresc) - 1);					//SCLL equation

	
	return (PRESC<<28 | SCLDEL<<20 | SDADEL<<16 | SCLH<<8 | SCLL<<0);
	
}

*/