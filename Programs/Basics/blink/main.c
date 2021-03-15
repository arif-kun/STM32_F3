#include "stm32f3xx.h"                  // Device header

void delayms(int count);


int main()
{
	unsigned int n = 0;
	
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
	
	GPIOE->MODER &=~(3<<20);
	GPIOE->MODER |= 1<<20;
	
	while(1)
	{
		GPIOE->ODR |= 1<<10;
		delayms(250);
		GPIOE->ODR &=~(1<<10);
		delayms(250);
		
	}
}





void delayms(int count)
{
	int i;
	SysTick->LOAD = SystemCoreClock/1000 - 1;		//8000 clock cycles in 1 ms
	SysTick->VAL = 0;														//reset current value to 0
	SysTick->CTRL = 5;													//clksource = internal. counter enabled. interrupt disabled
	
	for(i=0; i<count; i++)			//repeat 1ms cycles count number of times
	{
		while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)==0){}			//do nothing till Countflag becomes 1
	}
	
	SysTick->CTRL=0;		//disable counter
	
	
	
}