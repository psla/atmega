/*
 * slider_memory.h
 *
 * Created: 2015-03-29 14:51:18
 *  Author: Piotr
 */ 


#ifndef SLIDER_MEMORY_H_
#define SLIDER_MEMORY_H_

#include <avr/eeprom.h>

/// returns number of steps for a motor to take per slide
/// In theory, on new micro controller, this will return 0xFFFF, indicating that it was not even saved
/// On microcontroller which was used before, it might return bogus number.
uint16_t get_steps_per_slide();

/// saves total number of steps to get from one side of slider to another
void set_steps_per_slide(uint16_t steps_per_slide);

#endif /* SLIDER_MEMORY_H_ */