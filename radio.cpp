#include "radio.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 


#define NODEID 1
#define CRYPTPASS "0123456789012345"
#define NETWORKID 100
#define TXPOWER 31

RFM69 rfm69;

ISR(INT0_vect){
	RFM69::isr0();
}

void setup_interrupts() {
	// interrupt pin PD2
	DDRD &= ~(1 << PD2);
	
	PORTD |= (1 << PD2);

	// EIMSK  = 1 << INT0; // enable
	EIMSK  |= _BV(INT0); 
	
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
	SET_BIT(PORTB, PB6);
	_delay_ms(500);

	rfm69.initialize(RF69_915MHZ, NODEID, NETWORKID);

	rfm69.encrypt(CRYPTPASS);
	SET_BIT(PORTB, PB0);
	CLEAR_BIT(PORTB, PB6);
	rfm69.setPowerLevel(TXPOWER);
	SET_BIT(PORTB, PB6);
	
	while(1)
	{
		SET_BIT(PORTB, PB0);
		CLEAR_BIT(PORTB, PB6);
	}
}
