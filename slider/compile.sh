#!/bin/bash

avr-gcc -DF_CPU=8000000L -DMOTOR_PORT=PORTD -mmcu=atmega328p -std=c99 -Wall -pedantic -Os slider.c slider_memory.c ../lib/millis.c ../lib/lcd.c ../lib/stepper.c
