#ifndef _COMMON_h
#define _COMMON_h

#include <avr/interrupt.h>

#define SET_BIT(byte, bit) ((byte) |= (1UL << (bit)))
#define CLEAR_BIT(byte,bit) ((byte) &= ~(1UL << (bit)))
#define IS_SET(byte,bit) (((byte) & (1UL << (bit))) >> (bit))

void interrupts();

void noInterrupts();

#endif
