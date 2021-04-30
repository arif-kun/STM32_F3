#include "stm32f3xx.h"                  // Device header
#include "PC_Comm.h"
#include "MCP23XX.h"
#include "I2C_Driver.h"
#include <stdio.h>
#include "timer.h"
int main()
{
	uint8_t led = 0x00;
	volatile uint8_t data_in;
	RCC->AHBENR |= RCC_AHBENR_GPIODEN;
	GPIOD->MODER &=~(0x3<<4);
	GPIOD->MODER |= 1<<4;
	
	GPIOD->ODR &=~(1<<2);
	delayms(10);
	GPIOD->ODR |= 1<<2;
	PC_Comm_init(9600);
	
	I2C2_init();
	
	portex_setup();
	
	while(1)
	{
		data_in = portex_read(IO_B);

		portex_write(IO_A, data_in);
		delayms(1);
		
	}
	
	return 0;
	
}

