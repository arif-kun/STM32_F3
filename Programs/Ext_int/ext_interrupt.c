//Implements the on board button as an external interrupt to increase led_play function frequency

#include "stm32f3xx.h"                  // Device header
#include <math.h>

volatile int count = 1000;
int dbgcount = 0;
void delayms(int count);
void led_play(void);

int main()
{
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN | RCC_AHBENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;		//ENABLE clock to SYSCFG
	
	GPIOE->MODER &=~(0xFFFF0000);
	GPIOE->MODER |= 0X55550000;				//PE8-PE15 set as output
	
	GPIOA->MODER &=~(3<<0);
	SYSCFG->EXTICR[0] |= 1;			//PA0 ext int enabled
	
	EXTI->IMR |= 1;			//unmask ext interrupt 0
	
	EXTI->RTSR |= 1;			//rising edge trigger
	
	NVIC_EnableIRQ(EXTI0_IRQn);			//ENABLE ext int 0
	
	led_play();
}


void EXTI0_IRQHandler(void)
{
	count-=100;			//derease time by 100ms
	dbgcount++;
	EXTI->PR = 1;			//clear pending bit
}


void led_play(void)
{
	int c = 0;
	
	while(1)
	{
		GPIOE->ODR ^= ((int)pow(2,c++)<<8);
		delayms(count);
		if(c>8) c=0;
		if (count<=0) count=1000;
	}
}

void delayms(int count)
{
	int i;
	SysTick->LOAD = 8000-1;		//8000 cycles in 1 ms
	SysTick->VAL = 0;					//reset current value to 0
	SysTick->CTRL = 5;				//clksource = internal. counter enabled. interrupt disabled
	
	for(i=0; i<count; i++)			//repeat 1ms cycles count number of times
	{
		while((SysTick->CTRL & 0x10000)==0){}			//do nothing till Countflag becomes 1
	}
	
	SysTick->CTRL=0;		//disable counter
}