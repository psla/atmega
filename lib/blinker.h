#ifndef _BLINKER_h
#define _BLINKER_h

#include <avr/io.h>
#include <util/delay.h>
#include "common.h"

/**
  Blinks a double digit number

  It blinks first digit,
  Pauses
  Blinks the other digits.

  If number outside of range, it makes one long blink

  Uses LED for blinking
 */

void blink(uint8_t number, volatile uint8_t* port, uint8_t pin);

#endif
