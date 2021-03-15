#include "stm32f3xx.h"                  // Device header
#include "timer.h"

void delayms(int count)
{
	int i;
	SysTick->LOAD = SystemCoreClock/1000;		//16000 cycles in 1 ms
	SysTick->VAL = 0;					//reset current value to 0
	SysTick->CTRL = 5;				//clksource = internal. counter enabled. interrupt disabled
	//SysTick->CTRL &=~ (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk);
	//SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	
	for(i=0; i<count; i++)			//repeat 1ms cycles count number of times
	{
		while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));			//do nothing till Countflag becomes 1
	}
	
	SysTick->CTRL=0;		//disable counter
}