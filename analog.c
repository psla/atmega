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
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); 
	ADCSRA |= (1 << ADEN);

	uint16_t min = 1023;
	uint16_t max = 0;
	while(1) {
		// read PC0 (indexed from 0)
		ADMUX &= 0xf0;
		
		// or set PC1
		// ADMUX |= _BV(1);
		
		// sound detector on PC2
		ADMUX |= 0x02;
		
		// photoresistor now on PC3, we need 11 pins
		// ADMUX |= 0x03;

		// start conversion
		ADCSRA |= (1 << ADSC);
		
		// wait for conversion to finish
		while ( (ADCSRA & _BV(ADSC)) );
		//PORTB ^= _BV(PB0);
		
		uint16_t value = ADC;
		if(value < min) {
			min = value;
		}
		if(value > max) {
			max = value;
		}

		// photoresistor
		// if(ADC < 950) {
		// reed switch
		// if(ADC < 760) {
		// sound detector
		if(value < ((min + max) / 2)) {
			SET_BIT(PORTB, PB6);
			CLEAR_BIT(PORTB, PB7);
			_delay_ms(300);
		} else {
			SET_BIT(PORTB, PB7);
			CLEAR_BIT(PORTB, PB6);
		}
	}
}
