#include "PC_Comm.h"
#include "stm32f3xx.h"                  // Device header
#include <stdio.h>
#include <stdint.h>

void PC_Comm_init(int baudrate)
{
	int baud =(int) SystemCoreClock/baudrate;
	
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;			//enable clock to USART1
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;				//enable clock to GPIOC
	
	GPIOC->MODER &=~(3<<8 | 3<<10);
	GPIOC->MODER |= (2<<8 | 2<<10);						//PA4 and PA5 - AF
	
	GPIOC->AFR[0] &=~(0xF<<16 | 0xF<<20);
	GPIOC->AFR[0] |= (0x7<<16 | 0x7<<20);			//AF7 on PA4 and PA5
	
	
	USART1->CR1 &=~ (USART_CR1_UE);			//disable UART
	
	USART1->BRR = baud;								//Set baud rate
	
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;				//enable transmit and receive
	
	//USART1->CR1 |= USART_CR1_RXNEIE;
	
	USART1->CR1  |= USART_CR1_UE;				//enable UART
	
	
}

int send_char(char c)
{
	while(!(USART1->ISR & (USART_ISR_TXE)));
	
	USART1->TDR = c;
	
	while(!(USART1->ISR & USART_ISR_TC));
	
	return c;
	
}

int rec_char(void)
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
	c= rec_char();
	send_char(c);			//echo on screen what was entered
	if(c=='\r')			//if enter is pressed
	{
		send_char(c);					//print carriage return
		send_char('\n');			//print new line
	}
	
	return c;
} 


int fputc(int c, FILE *f)					// re-defining fputc for writing 
{
	return send_char(c);
}