#include "stm32f3xx.h"                  // Device header


#define RS_HI (GPIOD->ODR |= 1<<4)
#define RS_LOW (GPIOD->ODR &=~ (1<<4))

#define EN_HI (GPIOD->ODR |= 1<<6)
#define EN_LOW (GPIOD->ODR &=~ (1<<6))

#define RW_LOW (GPIOD->ODR &=~ (1<<5))

void delayms(int count);
void lcd_writecmd(char cmd);
void lcd_writedata(char data);
void lcd_init(void);
void lcd_writestring(char* str);

//PB4 - PB7


int main()
{
	lcd_init();
	
	//lcd_writecmd(0x80);
	
	
	while(1)
	{
		lcd_writecmd(0x80);
		delayms(1);
		lcd_writestring("Hello STM32!");
		delayms(1000);
		//lcd_writecmd(0x01);
	}
	
}

void delayms(int count)
{
	int i;
	SysTick->LOAD = 8000-1;		//16000 cycles in 1 ms
	SysTick->VAL = 0;					//reset current value to 0
	SysTick->CTRL = 5;				//clksource = internal. counter enabled. interrupt disabled
	
	for(i=0; i<count; i++)			//repeat 1ms cycles count number of times
	{
		while((SysTick->CTRL & 0x10000)==0){}			//do nothing till Countflag becomes 1
	}
	
	SysTick->CTRL=0;		//disable counter
	
	
	
}





void lcd_writecmd(char cmd)
{
	GPIOD->ODR &=~(0xF);			//PC0-PC3 = 0
	GPIOD->ODR |= ((cmd>>4) & 0xF);

	
	RS_LOW;
	RW_LOW;
	delayms(0);
	EN_HI;
	delayms(1);
	EN_LOW;
	delayms(2);
	
	
	
	GPIOD->ODR &=~(0xF);			//PC0-PC3 = 0
	GPIOD->ODR |= ((cmd) & 0xF);

	EN_HI;
	
	delayms(1);

	EN_LOW;
	delayms(10);
	
	
}

void lcd_writedata(char data)
{
	GPIOD->ODR &=~(0xF);			//PC0-PC3 = 0
	GPIOD->ODR |= (data>>4 & 0xF);

	
	RS_HI;
	RW_LOW;
	delayms(0);
	EN_HI;
	delayms(1);
	EN_LOW;
	delayms(2);
	
	
	
	GPIOD->ODR &=~(0xF);			//PC0-PC3 = 0
	GPIOD->ODR |= ((data) & 0xF);

	EN_HI;
	
	delayms(1);

	EN_LOW;
	delayms(10);
	
	
}

void lcd_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIODEN;
	
	
	GPIOD->MODER &=~(0x3FFF);
	GPIOD->MODER |= (0x1555);
	
	delayms(30);
	
	RS_LOW;
	RW_LOW;
	EN_LOW;
	delayms(10);
	
	lcd_writecmd(0x02);
	delayms(10);
	
	lcd_writecmd(0x28);
	delayms(10);
	
	lcd_writecmd(0x0C);			//clear display0
	delayms(10);
	
	lcd_writecmd(0x06);			//clear display
	delayms(10);
	
	lcd_writecmd(0x01);			//2 lines 5x7 characters
	delayms(10);
	
	
	lcd_writecmd(0x80);			//display on cursor off
	delayms(10);
	
	
	
}

void lcd_writestring(char* str)
{
	
	while(*str)
	{
		lcd_writedata(*(str++));
	}
}
