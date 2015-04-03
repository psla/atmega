/*
* Blinker.c
*
* Created: 2015-04-02 19:18:03
*  Author: Piotr
*/


#include <avr/io.h>
#include <util/delay.h>

// connect stepper motor via l293d to PD0-4


// this is the first pin out of 4 to drive motor.
// pins must be connected to the same port and must be consecutive.
#define MOTOR_FIRST_PIN PD0
#define MOTOR_PORT PORTD
#define MOTOR_DDR DDRD

// minimum two!
#define STEP_DELAY 2

int main(void)
{
	DDRB |= _BV(DDB7);
	DDRD |= _BV(DDD0);
	DDRD |= _BV(DDD1);
	DDRD |= _BV(DDD2);
	DDRD |= _BV(DDD3);
	
	/*
	while(1)
	{
	PORTB ^= _BV(PB7);
	_delay_ms(500);
	}
	*/
	// set up motor pins as output
	// MOTOR_DDR |= _BV(MOTOR_FIRST_PIN);
	// MOTOR_DDR |= _BV(MOTOR_FIRST_PIN + 1);
	// MOTOR_DDR |= _BV(MOTOR_FIRST_PIN + 2);
	// MOTOR_DDR |= _BV(MOTOR_FIRST_PIN + 3);
	
	while(1) {
		for(uint8_t i = 0; i < 50; i++) {
			// MOTOR_PORT = (MOTOR_PORT & (~0x0f)) | (motor_status);
			// MOTOR_PORT = 0x0C;
			MOTOR_PORT = 0b00000101;
			_delay_ms(STEP_DELAY);
			
			MOTOR_PORT = 0b00000110;
			_delay_ms(STEP_DELAY);
			
			MOTOR_PORT = 0b00001010;
			_delay_ms(STEP_DELAY);
			
			MOTOR_PORT = 0b00001001;
			_delay_ms(STEP_DELAY);
		}
		
		_delay_ms(3000);
	}
	
}
