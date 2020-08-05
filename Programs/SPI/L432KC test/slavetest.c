#include "stm32f3xx.h"                  // Device header

#define CS_LOW (GPIOA->ODR &=~(1<<4))
#define CS_HI (GPIOA->ODR |= 1<<4)


void SPI1_init();
void SPI1_writebyte(uint8_t data);
void delayms(int count);

int main()
{

	SPI1_init();
	
	CS_HI;
	
	
	while(1)
	{
		CS_LOW;
		
		SPI1_writebyte(1);
		
		CS_HI;
		
		delayms(500);
		//*****************************************************************
		
		CS_LOW;
		
		SPI1_writebyte(2);
		
		CS_HI;
		
		delayms(500);
		
		
		CS_LOW;
		
		SPI1_writebyte(3);
		
		CS_HI;
		
		delayms(500);
		
		
	}
}


void SPI1_init()
{
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	GPIOA->MODER &=~(3<<8 | 3<<10 | 3<<12 | 3<<14);
	GPIOA->MODER |= (1<<8 | 2<<10 | 2<<12 | 2<<14);
	
	GPIOA->AFR[0] &=~(0xF<<20 | 0xF<<24 | 0xF<<28);
	GPIOA->AFR[0] |= (5<<20 | 5<<24 | 5<<28);
	
	SPI1->CR1 &=~(SPI_CR1_SPE);
	SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;
	SPI1->CR1 |= 2<<3;				//1MHz baud rate
	
	SPI1->CR1 |= SPI_CR1_SPE;	
}

void SPI1_writebyte(uint8_t data)
{
	*(__IO uint8_t *)&SPI1->DR = data;
	
	while(!(SPI1->SR & SPI_SR_TXE));
	while(SPI1->SR & SPI_SR_BSY);
}


void delayms(int count)
{
	int i;
	SysTick->LOAD = 8000;		//16000 cycles in 1 ms
	SysTick->VAL = 0;					//reset current value to 0
	SysTick->CTRL = 5;				//clksource = internal. counter enabled. interrupt disabled
	
	for(i=0; i<count; i++)			//repeat 1ms cycles count number of times
	{
		while((SysTick->CTRL & 0x10000)==0){}			//do nothing till Countflag becomes 1
	}
	
	SysTick->CTRL=0;		//disable counter
	
}
