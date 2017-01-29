#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "../lib/common.h"

int
main (void)
{
	// LED demonstrating interrupts
	DDRD |= _BV(DDB0); 
	
	// LED demonstrating that button was presset
    DDRB |= _BV(DDB0); 

	// button (pull up)
    DDRB &= ~(1 << DDB1);
	PORTB |= (1 << DDB1);

	uint8_t counter = 0;

    while(1) 
    {
		if(IS_SET(PINB, PB1) == 1)
		{
			SET_BIT(PORTB, PB0);
		} 
		else{
			CLEAR_BIT(PORTB, PB0);
			counter = 0;
		}

		if(counter == 10) {
			CLEAR_BIT(PORTD, PD7);
		}

		if(counter == 20) {
			SET_BIT(PORTD, PD7);
			counter = 0;
		}

        _delay_ms(10);

		counter++;
    }
}
