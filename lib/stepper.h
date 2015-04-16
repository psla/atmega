/*
 * stepper.h
 *
 * Created: 2015-04-03 15:54:39
 *  Author: Piotr
 */ 


#ifndef STEPPER_H_
#define STEPPER_H_

#include <avr/io.h>

// For now you need to ensure that you are settings first 4 pins on MOTOR_PORT as output

/// Makes a one step in direction. Direction 0 or 1
/// It does not block after taking a step. It's a caller responsibility to call this method with appropriate frequency.
void onestep(uint8_t direction);

/// Takes number steps in specific direction. 
/// It will block until all steps are taken.
/// Use interval_between_steps to set interval in ms.
void step(uint8_t steps, uint8_t interval_between_steps, uint8_t direction);

/// Takes a number of steps in spefic direction - but will abort as soon as the pin value is equal to abort_level.
/// returns 1 when the condition is hit (port & mask are at abort level) or 0 if not reached.
/// 
/// It will block until all steps are taken.
/// Use interval_between_steps to set interval in ms.
uint8_t safe_step(uint8_t steps, uint8_t interval_between_steps, uint8_t direction, const uint8_t * port, uint8_t mask, uint8_t abort_level);

#endif /* STEPPER_H_ */