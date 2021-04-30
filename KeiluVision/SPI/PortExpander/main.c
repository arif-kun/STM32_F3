#include "stm32f3xx.h"                  // Device header
#include "SPI.h"

#define CS_LOW		(GPIOA->ODR &=~ (1<<4))
#define CS_HI			(GPIOA->ODR |=(1<<4))

#define SLAV_WR			0X40
#define	SLAV_RD			0X41
#define IO_A				0x09
#define	IO_B				0X19
#define IODIR_A			0X00
#define	IODIR_B			0X10	
#define IPOL_A			0x01
#define IPOL_B			0X11
#define INTCON_A		0X05
#define INTCON_B		0X15
#define GPINTEN_A		0X02
#define	GPINTEN_B		0X12


void delayms(int count);

int main()
{
	uint8_t led = 0;
	SPI_init(1,0,0,8);
	CS_HI;
	
	
	CS_LOW;
	
	SPI_transmit(0x40);
	SPI_transmit(0x0A);
	SPI_transmit(0x98);
		
	CS_HI;
	
	CS_LOW;
	
	SPI_transmit(0x40);
	SPI_transmit(0x00);
	SPI_transmit(0x00);
		
	CS_HI;
	
	while(1)
	{
		
		CS_LOW;
		
		SPI_transmit(0x40);
		SPI_transmit(0x09);
		SPI_transmit(led++);
		
		CS_HI;
		
		delayms(250);
	}
	
	
	return 0;
}





void delayms(int count)
{
	int i;
	SysTick->LOAD = 8000;		//16000 cycles in 1 ms
	SysTick->VAL = 0;					//reset current value to 0
	SysTick->CTRL = 5;				//clksource = internal. counter enabled. interrupt disabled
	
	for(i=0; i<count; i++)			//repeat 1ms cycles count number of times
	{
		while((SysTick->CTRL & 0x10000)==0){}			//do nothing till Countflag becomes 1
	}
	
	SysTick->CTRL=0;		//disable counter
	
}


