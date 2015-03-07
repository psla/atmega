#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

/*
	Connect two LEDs to PB0 and PB6, they will report temperature and humidity.
	Connect DHT11 to PB1
*/
int
main (void)
{
    // set outputs for port B
    DDRB |= _BV(DDB0); 
    DDRB |= _BV(DDB6); 
    
    while(1) 
    {
        _delay_ms(1000);
    }
}
