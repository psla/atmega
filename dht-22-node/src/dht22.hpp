#ifndef __DHT_22__
#define __DHT_22__

typedef struct Payload {
	uint8_t  nodeId; //store this nodeId
	int16_t  temperature;   
	int16_t  humidity;   
        uint8_t  checksum;
} Payload;

#endif
