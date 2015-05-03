#ifndef __DHT_22__
#define __DHT_22__

#include <stdint.h>

typedef struct Payload {
	uint8_t  nodeId; //store this nodeId
	// temperature is temperature * 10
	int16_t  temperature;   
	// humidity is humidity * 10
	int16_t  humidity;   
        uint8_t  checksum;
} Payload;

uint8_t get_checksum(const Payload* data);

#endif
