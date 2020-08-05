#include "stm32f3xx.h"                  // Device header
#include <stdint.h>
#include <math.h>

void delayms(int count);


int main()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
	
	GPIOE->MODER &=~(3<<20);
	GPIOE->MODER |= 1<<20;
	
	while(1)
	{
		GPIOE->ODR |= 1<<10;
		delayms(1000);
		GPIOE->ODR &=~(1<<10);
		delayms(1000);
		
	}
}


/*
int main()
{
	int n=0;
	int pos = 9;
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
	
	GPIOE->MODER &=~(0xFFFF0000);
	GPIOE->MODER |= 0X55550000;				//PE8-PE15 set as output
	
	while(1)
	{
		delayms(100);
		while(n<8)
		{
			GPIOE->ODR |= ((int)pow(2,n++)<<8);
			delayms(50);
		}
		
		GPIOE->ODR &=~(0xFF<<8);
		n = 0;

	}
}

*/



void delayms(int count)
{
	int i;
	SysTick->LOAD = 8000-1;		//16000 cycles in 1 ms
	SysTick->VAL = 0;					//reset current value to 0
	SysTick->CTRL = 5;				//clksource = internal. counter enabled. interrupt disabled
	
	for(i=0; i<count; i++)			//repeat 1ms cycles count number of times
	{
		while((SysTick->CTRL & 0x10000)==0){}			//do nothing till Countflag becomes 1
	}
	
	SysTick->CTRL=0;		//disable counter
	
	
	
}