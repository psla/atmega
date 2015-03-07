#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

int
main (void)
{
    DDRB |= _BV(DDB0); 
    DDRB |= _BV(DDB6); 
    
    while(1) 
    {
        PORTB ^= _BV(PB0);
        PORTB ^= _BV(PB6);
        _delay_ms(500);
    }
}
