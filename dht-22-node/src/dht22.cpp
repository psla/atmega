#include "dht22.hpp"

/** 
 * returns a checksum of payload
 *
 * checksum is computed by adding all bytes together as unsigned integer
 */
uint8_t get_checksum(const Payload* data) {
	uint8_t checksum = 0;

	// node Id
	checksum += *((const uint8_t*)data);

	// temperature
	checksum += *(((const uint8_t*)data) + 1);
	checksum += *(((const uint8_t*)data) + 2);

	// humidity
	checksum += *(((const uint8_t*)data) + 3);
	checksum += *(((const uint8_t*)data) + 4);

	return checksum;
}


