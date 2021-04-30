#include "stm32f3xx.h"                  // Device header


int main()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN;
	
	GPIOA->MODER &=~(3<<0);
	
	GPIOE->MODER &=~(3<<20);
	GPIOE->MODER |= 1<<20;
	
	while(1)
	{
		if(GPIOA->IDR & 0X1)
		{
			GPIOE->ODR |= 1<<10;
		}
		else
		{
			GPIOE->ODR &=~(1<<10);
		}
	}
}