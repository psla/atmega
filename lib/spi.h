#ifndef _SPI_h
#define _SPI_h

#define F_CPU 8000000UL

#include <avr/io.h>

void SPI_init(void);
char SPI_transmit(char cData);

#endif
