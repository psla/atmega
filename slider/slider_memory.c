/*
 * slider_memory.c
 *
 * Created: 2015-03-29 14:51:01
 *  Author: Piotr
 */ 

#include "slider_memory.h"

uint16_t EEMEM eemem_steps_per_slide;

uint16_t get_steps_per_slide() {
	return eeprom_read_word(&eemem_steps_per_slide);
}

void set_steps_per_slide(uint16_t steps_per_slide) {
	eeprom_update_word(&eemem_steps_per_slide, steps_per_slide);
}