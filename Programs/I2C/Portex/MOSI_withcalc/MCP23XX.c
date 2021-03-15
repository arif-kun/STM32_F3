#include "stm32f3xx.h"                  // Device header
#include "MCP23XX.h"
#include "I2C_Driver.h"


void portex_setup(void)
{
	portex_write(IODIRA, 0x00);
	portex_write(IODIRB, 0xFF);
	portex_write(IO_A, 0x00);
	portex_write(IPOLB, 0xFF);
}


void portex_write(uint8_t reg, uint8_t data)
{
	i2c_begin(PORTEX_WR,2);
	
	i2c_transmit(reg);
	
	i2c_transmit(data);
	
	while(!((I2C2->ISR & I2C_ISR_TC)));				//wait till tx complete flag is set
	
	i2c_stop();
}


uint8_t portex_read(uint8_t reg)
{
	volatile uint8_t reading;
	
	i2c_begin(PORTEX_WR,1);
	
	i2c_transmit(reg);
	
	i2c_begin(PORTEX_RD,1);
	
	reading = i2c_receive();
	
	i2c_stop();
	
	return reading;
}

