#include "stepper.h"
#include <avr/io.h>
#include "../lib/common.h"
#include <util/delay.h>

//
// This is a mock implementation of stepper
// which works on PB5 and PB6 LEDs

void step(uint8_t clockwise, uint8_t steps_count) {

  if(clockwise)
    SET_BIT(PORTB, PB5);
  SET_BIT(PORTB, PB6);
  while(--steps_count)
    _delay_ms(1);

  CLEAR_BIT(PORTB, PB5);
  CLEAR_BIT(PORTB, PB6);
}

void one_step(uint8_t clockwise) {

}
