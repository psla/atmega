#include "common.h"

void interrupts() {
	// reenable int0 interrupt
	EIMSK  |= _BV(INT0); // enable
}

void noInterrupts() {
	EIMSK  &= ~ (_BV(INT0)); // disable INT0 interrupt
}
