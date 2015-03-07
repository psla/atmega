#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "lib/dht11.h"

#define DHT11_DATA_BIT_COUNT 40
#define DHT11_PIN PINB
#define DHT11_DQ PINB1
#define DHTLIB_OK 2
#define DHTLIB_ERROR_CHECKSUM 3
#define DHTLIB_ERROR_TIMEOUT 4

/*
   Connect two LEDs to PB0 and PB6, they will report temperature and humidity.
   Connect DHT11 to PB2
   
   DHT library comes from https://github.com/goldsborough/AVR-DHT11
 */
	int
main (void)
{
	// set outputs for port B
	DDRB |= _BV(DDB0); 
	DDRB |= _BV(DDB6); 

	SET_BIT(PORTB, PB6);
	initDHT();
	uint8_t data [4];
	int8_t i;

	while(1) 
	{
		if(fetchData(data))
		{
			if(data[2] > 23 || data[2] < 15) {
				// too hot or too cold, impossible.
				SET_BIT(PORTB, PB6);
			}
			else {
				// within range
				CLEAR_BIT(PORTB, PB6);
			}
			for(i = 0; i < data[2]; i++)
			{
				SET_BIT(PORTB,LED);
				_delay_ms(100);

				CLEAR_BIT(PORTB,LED);
				_delay_ms(500);
			}
		} else {
			// error, lit up red led
			SET_BIT(PORTB,LED);
		}

		SET_BIT(PORTB, PB6);
		_delay_ms(5000);
	}
}



