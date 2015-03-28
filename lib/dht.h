// https://github.com/goldsborough/AVR-DHT11
#ifndef _DHT_h
#define _DHT_h

#define DHT_PORT_OUT PORTB
#define DHT_PORT_IN PINB
#define DHT_PIN PB1

#define LED PB0

#include <stdint.h>
#include "common.h"

void initDHT(void);

typedef struct dht11data_t {
	// decimal temperature 
	// i.e. 19 (19 celcius).
 	// non-negative!
	uint8_t temperature;

	// decimal humidity 
	// i.e. 41 (41%).
 	// non-negative!
	uint8_t humidity;
} dht11data;

typedef struct dht22data_t {
	// decimal + integral part of temperature
	// for instance, 35.1C will be represented as 351
	// this also can contain negative temperatures
 	// divide by 10 to get decimal 
	int16_t temperature;

	/// gets humidity with one integral place. Divide
        /// by 10 to get humidity
	uint16_t humidity;
} dht22data;

uint8_t fetchData(uint8_t* arr);

// updates data with DHT11 data
// returns 1 on success, 0 on failure (checksum error, etc)
uint8_t getDht11(dht11data * data);

// updates data with DHT22 data
// returns 1 on success, 0 on failure (checksum error, etc)
uint8_t getDht22(dht22data * data);
#endif
