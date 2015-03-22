#ifndef _STEPPER_H_
#define _STEPPER_H_

#include <avr/io.h>

/// Makes a specific amount of steps in the specific direction.
/// clockwise: 1 is clockwise, 0 is ccw
/// steps_count - how many steps to take. Not larger than 255.
/// it internally assumes a speed of rotation
void step(uint8_t clockwise, uint8_t steps_count);

/// Performs a one step in given direction
void one_step(uint8_t clockwise);

#endif
