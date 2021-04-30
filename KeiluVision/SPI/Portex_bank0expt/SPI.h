#include <stdint.h>
#include "stm32f3xx.h"                  // Device header

#ifndef _SPI_H_
#define _SPI_H_

#define MSTR	1
#define SLAVE	0

#define SPI_IS_BUSY(SPIx) (((SPIx)->SR & (SPI_SR_TXE | SPI_SR_RXNE)) == 0 || ((SPIx)->SR & SPI_SR_BSY))
#define SPI_WAIT(SPIx)            while (SPI_IS_BUSY(SPIx))

#define CS_HI			(GPIOA->ODR |= 1<<4)
#define CS_LOW		(GPIOA->ODR &=~(1<<4))


void SPI_init(uint8_t conf, uint8_t cpha, uint8_t cpol, uint8_t data_size);

void SPI_transmit(uint8_t data);

uint8_t SPI_rdwr(uint8_t data);

void SPI_tx_rx(uint8_t data, volatile uint8_t *buffer);


#endif		//_SPI_H_