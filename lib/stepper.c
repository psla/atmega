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

int lastStep = 3;

void release_holding_torque() {
	MOTOR_PORT &= 0xf0;
}

void set_motor_state() {
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

void onestep(uint8_t direction){
	// it could use in memory array indexed from 0-3, but
	// it would use memory in atmega. instead, use hard-coded values (that are compiled
	// into program, not use RAM).
	if(direction) {
		++lastStep;
	} else {
		lastStep += 3;
	}
	
	set_motor_state();
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

void wait(uint8_t wait_time) {
	uint8_t remaining_wait_time;
	
	for (remaining_wait_time = wait_time; remaining_wait_time > 0; --remaining_wait_time)
	{
		_delay_ms(1);
	}
}

uint8_t safe_step(uint8_t steps, uint8_t interval_between_steps, uint8_t direction, const uint8_t * port, uint8_t mask, uint8_t abort_level, uint8_t battery_saver)
{
	
	// initial check
	if((*port & mask) == abort_level) {
		return 1;
	}
	
	if(battery_saver) {
		// energize motor
		set_motor_state();
		wait(interval_between_steps);
	}
	
	for (uint8_t i = 0; i < steps; i++)
	{
		if((*port & mask) == abort_level) {
			return 1;
		}
		
		onestep(direction);
	
		wait(interval_between_steps);
	}
	
	if(battery_saver) {
		// energize motor
		release_holding_torque();
		wait(interval_between_steps);
	}
	
	return 0;
}