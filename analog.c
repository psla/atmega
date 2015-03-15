#define F_CPU 8000000UL

#include "lib/common.h"
#include <avr/io.h>
#include <util/delay.h>



/// This file plays with analog inputs
/// Connected:
/// PB0, 6, 7: 220Ohm + LED
/// PC0 to photo resistor (VC - 10k Ohm - Photo Resistor - GND), connect to photo resistor
/// PC1 to reed switch (VC - 1k Ohm - Reed switch - GND)
int main (void) {
	// set up diode outputs
	DDRB |= _BV(DDB0); 
	DDRB |= _BV(DDB6); 
	DDRB |= _BV(DDB7); 

	// enable analog input 
	// see http://www.avrbeginners.net/architecture/adc/adc.html for details
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN); 
	// ADCSRA |= (1 << ADEN);

	while(1) {
		// read PC0 (indexed from 0)
		ADMUX &= 0xf0;
		
		// or set PC1
		// ADMUX |= _BV(1);

		// start conversion
		ADCSRA |= (1 << ADSC);
		
		// wait for conversion to finish
		while ( (ADCSRA & _BV(ADSC)) );
		//PORTB ^= _BV(PB0);
		
		if(ADC < 950) {
		// if(ADC < 760) {
			SET_BIT(PORTB, PB6);
			CLEAR_BIT(PORTB, PB7);
		} else {
			SET_BIT(PORTB, PB7);
			CLEAR_BIT(PORTB, PB6);
		}
		//_delay_ms(500);
	}
}
