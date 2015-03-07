#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "lib/dht11.h"

/**
  Blinks a double digit number

  It blinks first digit,
  Pauses
  Blinks the other digits.

  If number outside of range, it makes one long blink

  Uses LED for blinking
 */

void blink_digit(uint8_t digit) {
	uint8_t i;
	if(digit == 0) {
		digit += 10;
	}
	
	for(i = 0; i < digit; i++) {
		SET_BIT(PORTB,LED);
		_delay_ms(100);

		CLEAR_BIT(PORTB,LED);
		_delay_ms(400);
	}
}

void blink(uint8_t number) {
	if(number < 10 || number > 100) {
		SET_BIT(PORTB, LED);
		_delay_ms(1000);
	}

	// blink first digit
	blink_digit((number / 10) % 10);

	_delay_ms(1000);

	// blink second digit
	blink_digit(number % 10);
}

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

	while(1) 
	{
		if(fetchData(data))
		{
			// blink temperature
			blink(data[2]);

			SET_BIT(PORTB, PB6);
			_delay_ms(300);
			CLEAR_BIT(PORTB, PB6);

			// blink humidity
			blink(data[0]);
		} else {
			// error, lit up red led
			SET_BIT(PORTB,LED);
		}

		SET_BIT(PORTB, PB6);
		_delay_ms(5000);
	}
}



