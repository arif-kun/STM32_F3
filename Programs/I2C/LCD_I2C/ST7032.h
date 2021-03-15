#ifndef __ST7032_H__
#define __ST7032_H__

#include <stdint.h>
#include "timer.h"

#define LCD_ADD		0x7C



void lcd_init(void);
void lcd_cmd(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_write(char* str);
void lcd_set_contrast(uint8_t contrast);
void display_control(uint8_t display_on, uint8_t cursor_on, uint8_t cursor_blink);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_clear(void);
#endif