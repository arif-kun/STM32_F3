#include "stm32f3xx.h"                  // Device header
#include "SPI.h"
#include "timer.h"
#include "MCP23XX.h"

#define RST		(GPIOB->ODR &=~(1<<0))
#define NRST	(GPIOB->ODR |= 1<<0)

void MCP23S17_setup(void);


int main()
{
	volatile uint8_t data_in, temp;
	uint8_t data_sav;
	uint8_t led = 0;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= 1<<0;
	RST;
	delayms(10);
	NRST;
	SPI_init(MSTR,0,0,8);
	CS_HI;
	
	MCP23S17_setup();
	//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	CS_LOW;
	
	SPI_transmit(PORTEX_WR);
	SPI_transmit(IODIRA);
	SPI_transmit(0x00);
		
	CS_HI;
	//--------------------------------------------------------------------
	
	while(1)
	{
		
		CS_LOW;
		
		SPI_transmit(PORTEX_WR);
		//delayms(1);
		SPI_transmit(IO_A);
		SPI_transmit(data_sav);
		
		CS_HI;
		
		
		CS_LOW;
		
		SPI_tx_rx(PORTEX_RD,&temp);
		SPI_tx_rx(IO_B,&temp);
		SPI_tx_rx(0x00,&data_in);				//dummy byte
		
		CS_HI;
		
		data_sav = data_in;
		//delayms(1);
		//___________________________________________________________________
		
		
		
		
	
		
		
		
	}
	
	
	return 0;
	
	
}




void MCP23S17_setup(void)
{
	//**********************************************************************	

//**********************************************************************
	
	CS_LOW;
	
	SPI_transmit(PORTEX_WR);
	SPI_transmit(IODIRA);		//0x00
	SPI_transmit(0x00);				//set whole GPIO Port A as output
		
	CS_HI;
//**********************************************************************

	CS_LOW;
		
	SPI_transmit(PORTEX_WR);
	SPI_transmit(IO_A);
	SPI_transmit(0x00);			//set output of Port A to 0 at start
		
	CS_HI;

//**********************************************************************

	CS_LOW;
	SPI_transmit(PORTEX_WR);
	SPI_transmit(IODIRB);
	SPI_transmit(0xFF);				//set whole GPIO Port B as input 
	
	CS_HI;
//**********************************************************************

	CS_LOW;
	SPI_transmit(PORTEX_WR);
	SPI_transmit(IPOLB);
	SPI_transmit(0xFF);					//read polarity of signals as opposite on PortB
	CS_HI;

}
