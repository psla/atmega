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

#endif /* STEPPER_H_ */