#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

// sleep library
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "../lib/common.h"
#include "../rfm2017/RFM69.h"

#define BUTTON_REGISTRY DDRD // is it input or output
#define BUTTON_PIN		PIND // reading input
#define BUTTON_PIN_INDEX	PD3 // index of the pin
#define BUTTON_PORT		PORTD // voltage levels for pins (pullup)

#define NODEID      4
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_915MHZ //Match this with the version of your Moteino! (others: RF69_433MHZ, RF69_868MHZ)
// TODO - deifne encryption
#define KEY         0 //has to be same 16 characters/bytes on all nodes, not more not less!

typedef struct Payload {
	uint8_t  nodeId; //store this nodeId
} Payload;


RFM69 radio(RF69_SPI_CS, RF69_IRQ_PIN, true, RF69_IRQ_NUM);

Payload dataToBeSent;

void blink_repeat(uint8_t n) {
	for(uint8_t i = 0; i < n; i++) {
		SET_BIT(PORTB, PB0);
		_delay_ms(350);
		CLEAR_BIT(PORTB, PB0);
		_delay_ms(150);
	}
}

void sleep() {
	// disable brown-out detection
	 set_sleep_mode(SLEEP_MODE_PWR_SAVE); 
	 cli();				
	 sleep_enable();		
	 sleep_bod_disable(); 
	 sei();				
	 sleep_cpu();			
	 sleep_disable();	
	 sei();
}

ISR (INT0_vect)
{
	// wakeup. Clear interrupts, and wait for another sleep.
	cli();
}

void ring_bell() {
	dataToBeSent.nodeId = NODEID;

	if (radio.sendWithRetry(GATEWAYID, (const void*)(&dataToBeSent), sizeof(dataToBeSent)))
	{
		// for testing you may uncomment line below
		// blink_repeat(1);
	}
	else
	{
		blink_repeat(8);
	}
}

int
main (void)
{
	// LED demonstrating interrupts
	DDRD |= _BV(DDB0); 
	
	// LED demonstrating that button was presset
    DDRB |= _BV(DDB0); 

	// button (pull up)
    BUTTON_REGISTRY &= ~(1 << BUTTON_PIN_INDEX);
	BUTTON_PORT |= (1 << BUTTON_PIN_INDEX);
	
	// interrupts for the button. Low level triggers interrupt
	EICRA |= (1 << ISC10);
	EIMSK |= (1 << INT1);

	// powersaving: disable ADC:
	ADCSRA &= ~(1 << ADEN);
	
	uint8_t counter = 0;
	
	const uint8_t blinks_before_sleep = 20;
	uint8_t blinks = 0;

	// enable diode during initialization
	SET_BIT(PORTB, PB0);
	
	radio.initialize(FREQUENCY,NODEID,NETWORKID);
	radio.setHighPower();
	radio.setPowerLevel(25);
	radio.encrypt(KEY);


    while(1) 
    {
		if(IS_SET(BUTTON_PIN, BUTTON_PIN_INDEX) == 1)
		{
			CLEAR_BIT(PORTB, PB0);
		} 
		else {
			SET_BIT(PORTB, PB0);
			if(blinks > 2) {
				blinks = 0;
				counter = 0;
			}
		}

		if(counter == 10) {
			CLEAR_BIT(PORTD, PD7);
		}

		if(counter == 20) {
			SET_BIT(PORTD, PD7);
			counter = 0;
			blinks++;
		}

		if(blinks == blinks_before_sleep) {
			CLEAR_BIT(PORTD, PD7);
			sleep();
			blinks = 0;
		}

        _delay_ms(10);

		counter++;
    }
}
