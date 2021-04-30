#include <stdint.h>

#ifndef _SPI_H_
#define _SPI_H_

void SPI_init(uint8_t conf, uint8_t cpha, uint8_t cpol, uint8_t data_size);

void SPI_transmit(uint8_t data);


#endif		//_SPI_H_