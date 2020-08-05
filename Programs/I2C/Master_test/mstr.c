#include "stm32f3xx.h"                  // Device header

void I2C2_init();
void i2c_write(uint8_t data);
void delayms(int count);

int main()
{

	I2C2_init();
	
	while(1)
	{
		i2c_write(1);
		delayms(500);
		
		i2c_write(2);
		delayms(500);
		
		i2c_write(3);
		delayms(500);
	}
	
}


void I2C2_init()
{
	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	GPIOA->MODER &=~(3<<18 | 3<<20);
	GPIOA->MODER |= 2<<18 | 2<<20;
	
	GPIOA->AFR[1] &=~(0XF<<4 | 0xF<<8);
	GPIOA->AFR[1] |= (4<<4 | 4<<8);
	
	GPIOA->OTYPER &=~(1<<9 | 1<<10);
	GPIOA->OTYPER |= 1<<9 | 1<<10;				//open drain
	
	I2C2->CR1 &=~(I2C_CR1_PE);
	
	I2C2->TIMINGR = 0x2030090B;			//1000ns tr, 100ns tf, 100khz
	
	I2C2->CR2 |= 0x40;			//slave address
	
	I2C2->CR2 &=~(I2C_CR2_NBYTES);
	
	I2C2->CR2 |= 1<<I2C_CR2_NBYTES_Pos;			//number of bytes to transfer
	
	I2C2->CR2 &=~(I2C_CR2_RD_WRN);
	
	I2C2->CR1 |= I2C_CR1_PE;
	
}


void i2c_write(uint8_t data)
{
	
	I2C2->CR2 |= I2C_CR2_START;			//generate start bit
	
	while(!((I2C2->ISR & I2C_ISR_TXE)));				//wait till TX buffer is empty

	I2C2->TXDR = data;

	while(!((I2C2->ISR & I2C_ISR_TC)));	
	
	I2C2->CR2 |= I2C_CR2_STOP;				//generate stop
	
}


void delayms(int count)
{
	int i;
	SysTick->LOAD = 8000;		//4000 cycles in 1 ms
	SysTick->VAL = 0;					//reset current value to 0
	SysTick->CTRL = 5;				//clksource = internal. counter enabled. interrupt disabled
	
	for(i=0; i<count; i++)			//repeat 1ms cycles count number of times
	{
		while((SysTick->CTRL & 0x10000)==0){}			//do nothing till Countflag becomes 1
	}
	
	SysTick->CTRL=0;		//disable counter
	
}