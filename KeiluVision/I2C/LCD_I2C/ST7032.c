#include "stm32f3xx.h"                  // Device header
#include "I2C_Driver.h"
#include "ST7032.h"
#include "timer.h"
#include <string.h>

void lcd_init(void)
{
	lcd_cmd(0x38);
	lcd_cmd(0x39);
	lcd_cmd(0x14);
	lcd_set_contrast(0x09);
	lcd_cmd(0x50);
	lcd_cmd(0x6C);
	delayms(250);
	display_control(1,0,0);
	lcd_clear();
	
}
	

void lcd_write(char* str)
{
	
	while(*(str))
	{
		lcd_data(*(str++));
	}
	
}

void lcd_cmd(uint8_t cmd)
{
	i2c_begin(LCD_ADD,2);
	i2c_transmit(0x00);
	i2c_transmit(cmd);
	i2c_stop();
	delayms(1);
	//delay
}

void lcd_data(uint8_t data)
{
	i2c_begin(LCD_ADD,2);
	i2c_transmit(0x40);
	i2c_transmit(data);
	i2c_stop();
	delayms(1);
	//delay
	
}

void lcd_set_contrast(uint8_t contrast)
{
	if(contrast>0xF) contrast = 0xF;
	lcd_cmd(0x70+contrast);						//0111 C3 C2 C1 C0
	
}




void lcd_set_cursor(uint8_t row, uint8_t col)
{
	if(row>2) row=2;
	if(col>16) col=16;
	
	switch(row)
	{
		case 1: lcd_cmd(0x80 + col - 1);
						break;
		
		case 2: lcd_cmd(0xC0 + col - 1);
						break;
	}
	
}

void display_control(uint8_t display_on, uint8_t cursor_on, uint8_t cursor_blink)
{
	lcd_cmd(0x08 + ((uint8_t)display_on<<2) + ((uint8_t)cursor_on<<1) + ((uint8_t)cursor_blink));
	
}

void lcd_clear(void)
{
	lcd_cmd(0x01);
	delayms(2);
}
	
