#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
 
struct 
int
main (void)
{
    DDRB |= _BV(DDB0);
    DDRB |= _BV(DDB6);
    DDRB |= _BV(DDB7);
    
    while(1) 
    
        PORTB ^= _BV(PB0);
        PORTB ^= _BV(PB6);
        PORTB ^= _BV(PB7);
        _delay_ms(500);
    }
}
