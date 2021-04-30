#include "stm32f3xx.h"                  // Device header

void I2C1_init();
void i2c_write(uint8_t regadd, uint8_t data);
void delayms(int count);

int main()
{
	uint8_t regadd = 0x00;
	uint8_t data = 0x00;
	I2C1_init();
	
	i2c_write(0x0A, 0x98);
	
	i2c_write(0x00,0x00);
	
	while(1)
	{
		i2c_write(0x09,data++);
		//data++;
		delayms(100);
	}
	
}


void I2C1_init()
{
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	
	GPIOB->MODER &=~(3<<12 | 3<<14);
	GPIOB->MODER |= 2<<12 | 2<<14;
	
	GPIOB->AFR[0] &=~(0XF<<24 | 0xF<<28);
	GPIOB->AFR[0] |= (4<<24 | 4<<28);
	
	GPIOB->OTYPER &=~(1<<6 | 1<<7);
	GPIOB->OTYPER |= 1<<6 | 1<<7;				//open drain
	
	I2C1->CR1 &=~(I2C_CR1_PE);
	
	I2C1->TIMINGR = 0x2030090B;			//1000ns tr, 100ns tf, 100khz
	
	I2C1->CR2 |= 0x40;			//slave address
	
	I2C1->CR2 &=~(I2C_CR2_NBYTES);
	
	I2C1->CR2 |= 2<<I2C_CR2_NBYTES_Pos;			//number of bytes to transfer
	
	I2C1->CR2 &=~(I2C_CR2_RD_WRN);
	
	I2C1->CR1 |= I2C_CR1_PE;
	
}


void i2c_write(uint8_t regadd, uint8_t data)
{
	
	I2C1->CR2 |= I2C_CR2_START;			//generate start bit
	
	
	//while(!((I2C1->ISR & I2C_ISR_TXE)==I2C_ISR_TXE));
	while(!((I2C1->ISR & I2C_ISR_TXE)));
	I2C1->TXDR = regadd;
	
	//while(!((I2C1->ISR & I2C_ISR_TXE)==I2C_ISR_TXE));
	while(!((I2C1->ISR & I2C_ISR_TXE)));

	I2C1->TXDR = data;
	
	//while(!((I2C1->ISR & I2C_ISR_TC) == (I2C_ISR_TC)));			//wait for previous transfer to complete
	while(!((I2C1->ISR & I2C_ISR_TC)));	
	
	I2C1->CR2 |= I2C_CR2_STOP;				//generate stop
	
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