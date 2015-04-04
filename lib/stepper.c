/*
* stepper.c
*
* Created: 2015-04-03 15:54:29
*  Author: Piotr
*/

#include "stepper.h"
#include <util/delay.h>

#ifndef MOTOR_PORT
#error "MOTOR_PORT not defined! Specify PORT into which motor is connected."
#endif


void onestep(uint8_t direction){
	// it could use in memory array indexed from 0-3, but
	// it would use memory in atmega. instead, use hard-coded values (that are compiled
	// into program, not use RAM).
	static int lastStep = 3;
	if(direction) {
		++lastStep;
	} else {
		lastStep += 3;
	}
	
	lastStep %= 4;
	
	switch(lastStep){
		case 0:
		MOTOR_PORT = (MOTOR_PORT & 0xf0) | 0b00000101;
		break;
		case 1:
		MOTOR_PORT = (MOTOR_PORT & 0xf0) | 0b00000110;
		break;
		case 2:
		MOTOR_PORT = (MOTOR_PORT & 0xf0) | 0b00001010;
		break;
		case 3:
		MOTOR_PORT = (MOTOR_PORT & 0xf0) | 0b00001001;
		break;
	}
}

void step(uint8_t steps, uint8_t interval_between_steps, uint8_t direction) {
	uint8_t wait_time;
	
	for (uint8_t i = 0; i < steps; i++)
	{
		wait_time = interval_between_steps;
		onestep(direction);
		
		for (wait_time = interval_between_steps; wait_time > 0; --wait_time)
		{
			_delay_ms(1);
		}
	}
}