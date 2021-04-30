#include "stm32f3xx.h"                  // Device header
#include "I2C_Driver.h"
#include "timer.h"
#include "ST7032.h"

int main()
{
	int i=0;
	I2C2_init();
	lcd_init();
	
	lcd_set_cursor(1,1);
	lcd_write("Contrast test...");
	
	for(i=0; i<=0xF; i++)
	{
		lcd_set_contrast(i);
		delayms(250);
	}
	
	lcd_clear();
	
	lcd_set_contrast(0x09);
	
	while(1)
	{
		lcd_set_cursor(1,6);
		lcd_write("Hello");
		
	}
	
	return 0;
}