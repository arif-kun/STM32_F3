#include "stm32f3xx.h"                  // Device header
#include <stdio.h>

void UART1_init(void);
void delayms(int count);
int uart1_chartx(char c);
void uart1_stringtx(char* string);
char uart_rx_char(void);


int main()
{
	char option;
	char freq;
	UART1_init();
	GPIOE->MODER &=~(0xFFFF0000);
	GPIOE->MODER |= 0X55550000;				//PE8-PE15 set as output
	
	freq = SystemCoreClock/1000000;

	while(1)
	{
		printf("\nSys Clock = %d MHz\r",freq);
		delayms(500);
		//uart1_stringtx("\n\r\n\rPress R/G/B/O  : ");
		/*
		option = uart_rx_char();
		//uart1_chartx(option);
		
		switch(option)
		{
			case 'R':
			case 'r': GPIOE->BSRR = 0xDD002200;
			break;
			
			case 'B':
			case 'b': GPIOE->BSRR = 0xEE001100;
			break;
			
			case 'G':
			case 'g': GPIOE->BSRR = 0x77008800;
			break;
			
			case 'O':
			case 'o' : GPIOE->BSRR = 0xBB004400;
			break;
			
			default : uart1_stringtx("\n\rPlease enter a valid character!\n\r");
			break;
		}
		*/
		
	}
	
}

void UART1_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	RCC->AHBENR |= RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOEEN;
	
	GPIOC->MODER &=~(3<<8 | 3<<10);
	GPIOC->MODER |= (2<<8 | 2<<10);
	

	
	
	GPIOC->AFR[0] |= (0x7<<16 | 0x7<<20);
	
	
	USART1->CR1 &=~ (USART_CR1_UE);			//disable UART
	
	USART1->BRR = 0x341;								//Set 9600 baud at 4MHz clock freq
	
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;				//enable transmit
	
	USART1->CR1 |= USART_CR1_UE;				//enable UART
	
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

int uart1_chartx(char c)
{
	while(!(USART1->ISR & (USART_ISR_TXE)));
	
	USART1->TDR = c;
	
	while(!(USART1->ISR & USART_ISR_TC));
	//while((USART1->SR & (1<<6))==0);
	return c;
}

void uart1_stringtx(char* string)
{
	while(*string)
	{
		uart1_chartx(*(string++));
		//delayms(1);
	}
	
}

char uart_rx_char(void)
{
	char c;
	while(!(USART1->ISR & USART_ISR_RXNE));		//wait till receive buffer is NOT empty
	c = USART1->RDR;
	return c;
}





struct __FILE									//__FILE__ is a preprocessor macro that expands to full path to the current file
{
	int handle;		//handler
};

//Initializing standard streams - stdin, stdout, stderror
FILE __stdin;			
FILE __stdout;
FILE __stderr;



// re-defining fgetc for reading input
int fgetc(FILE *f)												//f is a pointer to current file 
{
	int c;
	c= uart_rx_char();
	if(c=='\r')			//if enter is pressed
	{
		uart1_chartx(c);					//print carriage return
		c = '\n';
	}
	
	uart1_chartx(c);				//print new line
	
	return c;
} 

int fputc(int c, FILE *f)					// re-defining fputc for writing 
{
	return uart1_chartx(c);
}


