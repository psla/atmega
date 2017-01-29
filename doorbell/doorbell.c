#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "../lib/common.h"

int
main (void)
{
    DDRB |= _BV(DDB0); 
    DDRB &= ~(1 << DDB1);
	PORTB |= (1 << DDB1);

    while(1) 
    {
		if(IS_SET(PINB, PB1) == 1)
		{
			SET_BIT(PORTB, PB0);
		} 
		else{
			CLEAR_BIT(PORTB, PB0);
		}

        _delay_ms(500);
    }
}
