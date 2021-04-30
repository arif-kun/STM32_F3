#include "SPI.h"
#include "stm32f3xx.h"                  // Device header
#include <stdint.h>


void SPI_init(uint8_t conf, uint8_t cpha, uint8_t cpol, uint8_t data_size)
{
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;				//Enable SPI1
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	GPIOA->MODER &=~(3<<8 | 3<<10 | 3<<12 | 3<<14);  

	GPIOA->AFR[0] &=~(0xF<<20 | 0xF<<24 | 0xF<<28);

	
	if(conf)
	{
		GPIOA->MODER |= (1<<8 | 2<<10 | 2<<12 | 2<<14);
		GPIOA->AFR[0] |= (5<<20 | 5<<24 | 5<<28);
		
		SPI1->CR1 &=~(SPI_CR1_SPE);				//disable the controller
		
		SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;							//conf as master, software slave select
		
		
		SPI1->CR1 |= 0x2<<3;			//baud rate = fpclk/8
		
		//SPI1->CR2 = 0x7<< SPI_CR2_DS_Pos;		// 8bit transfers
		SPI1->CR2 |= SPI_CR2_FRXTH;
		SPI1->CR1 |= SPI_CR1_SPE;					//enable controller
	}
	
	else
	{
		GPIOA->MODER |= (2<<8 | 2<<10 | 2<<12 | 2<<14);
		GPIOA->AFR[0] |= (5<<16 |5<<20 | 5<<24 | 5<<28);
		
		SPI1->CR1 &=~(SPI_CR1_SPE);				//disable the controller
		
		SPI1->CR1 &=~ (SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM);							//conf as slave, disable software slave select
		SPI1->CR1 |=(cpol<<SPI_CR1_CPOL_Pos) | (cpha<<SPI_CR1_CPHA_Pos);		//cphase and cpol
		SPI1->CR1 &=~ SPI_CR1_LSBFIRST;																		  //MSB first
		
		SPI1->CR1 |= 0x2<<SPI_CR1_BR_Pos;			//baud rate = fpclk/8
		
		SPI1->CR2 = data_size << SPI_CR2_DS_Pos;		
		
		SPI1->CR1 |= SPI_CR1_SPE;
	}
	
}


void SPI_transmit(uint8_t data)
{

	
	while(!(SPI1->SR & SPI_SR_TXE));
	*(__IO uint8_t *)&SPI1->DR = data;
	
	while(SPI1->SR & SPI_SR_BSY);
	

}


uint8_t SPI_rdwr(uint8_t data)
{
	volatile uint8_t f;
	while(!(SPI1->SR & SPI_SR_TXE));
	*(__IO uint8_t *)&SPI1->DR = data;
	
	
	while(SPI1->SR & SPI_SR_BSY);

	
	while(!(SPI1->SR & SPI_SR_RXNE));			//wait till RX buffer in not empty
	f = *(volatile uint8_t *)&SPI1->DR;
	return f;
}


void SPI_tx_rx(uint8_t data, volatile uint8_t *buffer)
{

	while(!(SPI1->SR & SPI_SR_TXE));				// wait till TX buffer is empty
	*(volatile uint8_t *)&SPI1->DR = data;				//send data
	//while(!(SPI1->SR & SPI_SR_TXE));				// wait till TX buffer is empty
	while(SPI1->SR & SPI_SR_BSY);
	while(!(SPI1->SR & SPI_SR_RXNE));				//wait till RX buffe is NOT empty	
	
	*buffer = *(volatile uint8_t *)&SPI1->DR;
	
	//while(SPI1->SR & SPI_SR_BSY);				   // wait if SPI line is busy	

	
	
}
