#include "blinker.h"

/**
  Blinks a double digit number

  It blinks first digit,
  Pauses
  Blinks the other digits.

  If number outside of range, it makes one long blink

  Uses LED for blinking
 */

void blink_digit(uint8_t digit, volatile uint8_t* port, uint8_t pin) {
	uint8_t i;
	if(digit == 0) {
		digit += 10;
	}
	
	for(i = 0; i < digit; i++) {
		SET_BIT((*port), pin);
		_delay_ms(100);

		CLEAR_BIT(*(port), pin);
		_delay_ms(400);
	}
}

void blink(uint8_t number, volatile uint8_t* port, uint8_t pin) {
	if(number < 10 || number > 100) {
		SET_BIT(*(port), pin);
		_delay_ms(1000);
		CLEAR_BIT(*(port), pin);
		return;
	}

	// blink first digit
	blink_digit((number / 10) % 10, port, pin);

	_delay_ms(1000);

	// blink second digit
	blink_digit(number % 10, port, pin);
}
