#include "stm32f3xx.h"                  // Device header
#include "PC_Comm.h"
#include <stdio.h>
#include <math.h>

void delayms(int count)
{
	int i;
	SysTick->LOAD = SystemCoreClock/1000 - 1;		//16000 cycles in 1 ms
	SysTick->VAL = 0;													 //reset current value to 0
	SysTick->CTRL = 5;												 //clksource = internal. counter enabled. interrupt disabled
	
	for(i=0; i<count; i++)											//repeat 1ms cycles count number of times
	{
		while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)==0){}			//do nothing till Countflag becomes 1
	}

	SysTick->CTRL=0;		//disable counter
}


int main()
{	
	int num;
	PC_Comm_init(115200);			//initialize pc comms with 115200 baud rate
	
	
	while(1)
	{
		printf("\n\rHello STM32 ");
		delayms(500);
		
	}
	
}