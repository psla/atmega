#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "lib/dht11.h"
#include "lib/blinker.h"


/*
   Connect two LEDs to PB0 and PB6, they will report temperature and humidity.
   Connect DHT11 to PB1

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

	while(1) 
	{
		if(fetchData(data))
		{
			// blink temperature
			blink(data[2], &PORTB, LED);

			SET_BIT(PORTB, PB6);
			_delay_ms(300);
			CLEAR_BIT(PORTB, PB6);

			// blink humidity
			blink(data[0], &PORTB, LED);
		} else {
			// error, lit up red led
			SET_BIT(PORTB,LED);
		}

		SET_BIT(PORTB, PB6);
		_delay_ms(5000);
	}
}



