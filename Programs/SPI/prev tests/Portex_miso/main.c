#include "stm32f3xx.h"                  // Device header

#define CS_LOW (GPIOB->ODR &=~(1<<12))
#define CS_HI (GPIOB->ODR |= 1<<12)

volatile uint8_t data_in;

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


void SPI1_init();
void SPI2_init();
void SPI_writebyte(uint8_t data);
void delayms(int count);
void MCP23S17_setup(void);

int SPI_readbyte(void);

uint8_t SPI_rdwr(int data);

volatile uint8_t f;;

int main()
{
	volatile uint8_t x;
	__disable_irq();
	
	SPI2_init();
	CS_HI;
	MCP23S17_setup();
	NVIC_EnableIRQ(SPI2_IRQn);
	
	
	//__enable_irq();				// enable global interrupts
	
	while(1)
	{
		
		CS_LOW;
		
		SPI_rdwr(SLAV_RD);
		SPI_rdwr(IO_B);
		x = SPI_rdwr(0x00);				//dummy byte
		
		CS_HI;
		
		//___________________________________________________________________
		
		CS_LOW;
		
		SPI_writebyte(SLAV_WR);
		SPI_writebyte(IO_A);
		SPI_writebyte(x);
		
		CS_HI;
		
		
	
		
		
		
	}
}


/*
void SPI2_IRQHandler(void)
{
	if(SPI2->SR & SPI_SR_RXNE)
	{
		data_in = *(__IO uint8_t *)&SPI2->DR;
		
		CS_LOW;
		
		SPI_writebyte(SLAV_WR);
		SPI_writebyte(IO_A);
		SPI_writebyte(data_in);
		
		CS_HI;
	}
	


	
}
*/


uint8_t SPI_rdwr(int data)
{
	while(!(SPI2->SR & SPI_SR_TXE));
	*(__IO uint8_t *)&SPI2->DR = data;
	
	
	while(SPI2->SR & SPI_SR_BSY);

	
	while(!(SPI2->SR & SPI_SR_RXNE));			//wait till RX buffer in not empty
	f = *(__IO uint8_t *)&SPI2->DR;
	return f;
}




void SPI_writebyte(uint8_t data)
{
  while(!(SPI2->SR & SPI_SR_TXE));
	*(__IO uint8_t *)&SPI2->DR = data;
	
	while(SPI2->SR & SPI_SR_BSY);
}

/*
int SPI_readbyte(void)
{
	while(!(SPI1->SR & SPI_SR_RXNE));
	while(SPI1->SR & SPI_SR_BSY);
	data_in = *(__IO uint8_t *)&SPI1->DR;
	
	return data_in;
	
}
*/


void delayms(int count)
{
	int i;
	SysTick->LOAD = 8000;		//8000 cycles in 1 ms
	SysTick->VAL = 0;					//reset current value to 0
	SysTick->CTRL = 5;				//clksource = internal. counter enabled. interrupt disabled
	
	for(i=0; i<count; i++)			//repeat 1ms cycles count number of times
	{
		while((SysTick->CTRL & 0x10000)==0){}			//do nothing till Countflag becomes 1
	}
	
	SysTick->CTRL=0;		//disable counter
	
}


void SPI1_init()
{
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	GPIOA->MODER &=~(3<<8 | 3<<10 | 3<<12 | 3<<14);
	GPIOA->MODER |= (1<<8 | 2<<10 | 2<<12 | 2<<14);		//GPIO4 -manual NSS, GPIO5,6,7 - AF
	
	GPIOA->AFR[0] &=~(0xF<<20 | 0xF<<24 | 0xF<<28);
	GPIOA->AFR[0] |= (5<<20 | 5<<24 | 5<<28);					//select PA5,PA6,PA7 AFs (SPI1)
	
	SPI1->CR1 &=~(SPI_CR1_SPE);				//disable SPI
	SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;			//configure as mstr and select internal slave select
	SPI1->CR1 |= 2<<3;							//1 Mhz
	
	//SPI1->CR2 |= SPI_CR2_RXNEIE;			//enable SPI1 rxne interrupt
	SPI1->CR1 |= SPI_CR1_SPE;				//enable SPI
}


void SPI2_init()
{
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	
	GPIOB->MODER &=~(3<<24 | 3<<26 | 3<<28 | 3<<30);
	GPIOB->MODER |= 1<<24 | 2<<26 | 2<<28 | 2<<30;
	
	GPIOB->AFR[1] &=~(0xF<<20 | 0xF<<24 | 0xF<<28);
	GPIOB->AFR[1] |= 5<<20 | 5<<24 | 5<<28;
	
	SPI2->CR1 &=~(SPI_CR1_SPE);
	SPI2->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;
	SPI2->CR1 |= 1<<3;
	//SPI2->CR1 |= SPI_CR2_RXNEIE;
	SPI2->CR2 |= SPI_CR2_FRXTH;
	
	SPI2->CR1 |= SPI_CR1_SPE;
	
}


void MCP23S17_setup(void)
{
	//**********************************************************************	
	CS_LOW;
	
	SPI_writebyte(SLAV_WR);
	SPI_writebyte(0x0A);			//access IOCON register
	SPI_writebyte(0x98);			//unpair GPIO A an B for 8 bit mode
		
	CS_HI;
//**********************************************************************
	
	CS_LOW;
	
	SPI_writebyte(SLAV_WR);
	SPI_writebyte(IODIR_A);		//0x00
	SPI_writebyte(0x00);				//set whole GPIO Port A as output
		
	CS_HI;
//**********************************************************************

	CS_LOW;
		
	SPI_writebyte(SLAV_WR);
	SPI_writebyte(IO_A);
	SPI_writebyte(0x00);			//set output of Port A to 0 at start
		
	CS_HI;

//**********************************************************************

	CS_LOW;
	SPI_writebyte(SLAV_WR);
	SPI_writebyte(IODIR_B);
	SPI_writebyte(0xFF);				//set whole GPIO Port B as input 
	
	CS_HI;
//**********************************************************************

	CS_LOW;
	SPI_writebyte(SLAV_WR);
	SPI_writebyte(IPOL_B);
	SPI_writebyte(0xFF);					//read polarity of signals as opposite on PortB
	CS_HI;

}

