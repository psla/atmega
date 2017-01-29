#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

// sleep library
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "../lib/common.h"

#define BUTTON_REGISTRY DDRD // is it input or output
#define BUTTON_PIN		PIND // reading input
#define BUTTON_PIN_INDEX	PD2 // index of the pin
#define BUTTON_PORT		PORTD // voltage levels for pins (pullup)

void sleep() {
	// disable brown-out detection
	 set_sleep_mode(SLEEP_MODE_PWR_SAVE); 
	 cli();				
	 sleep_enable();		
	 sleep_bod_disable(); 
	 sei();				
	 sleep_cpu();			
	 sleep_disable();	
	 sei();
}

void wakeup() {
	cli();
}

int
main (void)
{
	// LED demonstrating interrupts
	DDRD |= _BV(DDB0); 
	
	// LED demonstrating that button was presset
    DDRB |= _BV(DDB0); 

	// button (pull up)
    BUTTON_REGISTRY &= ~(1 << BUTTON_PIN_INDEX);
	BUTTON_PORT |= (1 << BUTTON_PIN_INDEX);

	// powersaving: disable ADC:
	ADCSRA &= ~(1 << ADEN);

	uint8_t counter = 0;
	
	const uint8_t blinks_before_sleep = 20;
	uint8_t blinks = 0;

    while(1) 
    {
		if(IS_SET(BUTTON_PIN, BUTTON_PIN_INDEX) == 1)
		{
			SET_BIT(PORTB, PB0);
		} 
		else {
			CLEAR_BIT(PORTB, PB0);
			counter = 0;
		}

		if(counter == 10) {
			CLEAR_BIT(PORTD, PD7);
		}

		if(counter == 20) {
			SET_BIT(PORTD, PD7);
			counter = 0;
			blinks++;
		}

		if(blinks == blinks_before_sleep) {
			CLEAR_BIT(PORTD, PD7);
			sleep();
		}

        _delay_ms(10);

		counter++;
    }
}
