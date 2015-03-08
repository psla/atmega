#include "radio.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 

RFM69 rfm69;

ISR(INT0_vect){
	RFM69::isr0();
}

void setup_interrupts() {
	// interrupt pin PD2
	DDRD &= ~(1 << PD2);
	
	PORTD |= (1 << PD2);

	// EIMSK  = 1 << INT0; // enable
	EIMSK  |= _BV(INT0); // enable
	
	// trigger on raising edge
	EICRA |= (1 << ISC00) | (1 << ISC01);

	// enable global interrupts
	sei();
}

int main (void) {
	setup_interrupts();
	millis_init();
	DDRB |= _BV(DDB0); 
	DDRB |= _BV(DDB6); 

	SET_BIT(PORTB, PB0);
	// rfm69.initialize(RF69_915MHZ, 5, 5);
	
	
	
	uint8_t state = 2;
	uint8_t i = 0;
	while(1)
	{
		
	}
}
