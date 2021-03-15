#include "stm32f3xx.h"                  // Device header


int main()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOEEN;
	
	GPIOA->MODER &=~(3<<0);		//set PA0 as input
	
	GPIOE->MODER &=~(3<<18);		//reset PE9 
	GPIOE->MODER |= 1<<18;			//set PE9 as output
	
	while(1)
	{
		if(GPIOA->IDR & 1<<0)		//if button is pressed
		{
			GPIOE->ODR |= 1<<9;
		}
		else
		{
			GPIOE->ODR &=~(1<<9);
		}
	}
	
	return 0;
}
