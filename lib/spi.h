#ifndef _SPI_h
#define _SPI_h

#include <avr/io.h>

void SPI_init(void);
char SPI_transmit(char cData);

#endif
