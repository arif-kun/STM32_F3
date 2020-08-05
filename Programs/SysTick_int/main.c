#include "stm32f3xx.h"                  // Device header
#include <math.h>

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
	//GPIOE->ODR ^= 1<<14 | 1<<10;
	static int c = 0;
	
	/*
	switch(c)
		{

			case 4: GPIOE->BSRR = 0xDD002200;
			break;
			

			case 3: GPIOE->BSRR = 0xEE001100;
			break;
			

			case 2: GPIOE->BSRR = 0x77008800;
			break;
			

			case 1: GPIOE->BSRR = 0xBB004400;
			break;
			
			//default : GPIOE->BSRR = 0xFF000000;
			//break;
		}
		
		c++;
		if(c>4) c=0;
		*/
		
		GPIOE->ODR ^= ((int)pow(2,c++)<<8);
		
		if(c>8) c=0;
	
}




void systimer_event(int count)
{
	SysTick->LOAD = count - 1;
	SysTick->VAL = 0;
	SysTick->CTRL = 7;
}