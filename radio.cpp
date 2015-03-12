#include "radio.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 


#define NODEID 99
#define CRYPTPASS "0123456789012345"
#define NETWORKID 100
#define TXPOWER 31

RFM69 rfm69 = RFM69(PB2, PD2, true, 0);

typedef struct {
  int           nodeId; //store this nodeId
  unsigned long uptime; //uptime in ms
  float         temp;   //temperature maybe?
} Payload;
Payload theData;

ISR(INT0_vect){
	RFM69::isr0();
}

void setup_interrupts() {
	// interrupt pin PD2
	// set port PD2 as input
	DDRD &= ~(1 << PD2);
	
	// enable pull-up resistor
	PORTD |= (1 << PD2);

	// EIMSK  = 1 << INT0; // enable
	EIMSK  |= _BV(INT0); 
	
	// trigger on raising edge
	EICRA |= (1 << ISC00) | (1 << ISC01);

	// enable global interrupts
	sei();
}

void blink_repeat(uint8_t n) {
	for(uint8_t i = 0; i < n; i++) {
		CLEAR_BIT(PORTB, PB0);
		_delay_ms(70);
		SET_BIT(PORTB, PB0);
		_delay_ms(70);
	}
}

int main (void) {
	setup_interrupts();

	DDRB |= _BV(DDB0); 
	DDRB |= _BV(DDB6); 

	SET_BIT(PORTB, PB0);
	SET_BIT(PORTB, PB6);

	// activate-pull-up resistor
	SET_BIT(PORTB, PB2);
	_delay_ms(500);

	rfm69.initialize(RF69_915MHZ, NODEID, NETWORKID);
	CLEAR_BIT(PORTB, PB6);
        rfm69.setHighPower(); //uncomment only for RFM69HW!
	rfm69.encrypt(0);
	CLEAR_BIT(PORTB, PB0);

	_delay_ms(500);
	SET_BIT(PORTB, PB0);
	SET_BIT(PORTB, PB6);

	_delay_ms(500);

	// rfm69.setPowerLevel(TXPOWER);
	// rfm69.rcCalibration();

	CLEAR_BIT(PORTB, PB6);
	
	uint8_t counter = 0;
	while(1)
	{
		if(counter % 2 == 0) {
			CLEAR_BIT(PORTB, PB0);
			// SET_BIT(PORTB, PB6);
		} else {
			//CLEAR_BIT(PORTB, PB6);
			SET_BIT(PORTB, PB0);
		}

	    //fill in the struct with new values
	    theData.nodeId = NODEID;
	    theData.uptime = 123;
	    theData.temp = 91.23; //it's hot!
		// rfm69.send(1, (const void *) CRYPTPASS, 12, false);
	        if (rfm69.sendWithRetry(1, (const void*)(&theData), sizeof(theData)))
			blink_repeat(2);
		else
			blink_repeat(7);

		_delay_ms(500);
		counter++;
	}
}
