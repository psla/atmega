// https://github.com/goldsborough/AVR-DHT11
#ifndef _DHT_11_h
#define _DHT_11_h

#define F_CPU 8000000UL

#define DHT_PORT_OUT PORTB
#define DHT_PORT_IN PINB
#define DHT_PIN PB1

#define LED PB0

#include <stdint.h>
#include "common.h"

void initDHT(void);

uint8_t fetchData(uint8_t* arr);

#endif
