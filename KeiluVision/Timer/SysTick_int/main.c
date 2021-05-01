#include "stm32f3xx.h"                  // Device header
#include <tgmath.h>

void systimer_event(int count);
void LEDplay(int n);

int main()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
	
	GPIOE->MODER &=~(0xFFFF0000);
	GPIOE->MODER |= 0X55550000;				//PE8-PE15 set as output
	
	//systimer_event(8000000);
	systimer_event(1000000/2);
	
	while(1);
}


void SysTick_Handler(void) 
{ 
	
	static int c = 0;
	
		
		
		GPIOE->ODR ^= ((int)powf(2,c++)<<8);
		
	//GPIOE->ODR += 1<<8;
	
		if(c>8)
		{
			c = 0;
			
		}
	
}




void systimer_event(int count)
{
	SysTick->LOAD = count - 1;
	SysTick->VAL = 0;
	SysTick->CTRL = 7;
}