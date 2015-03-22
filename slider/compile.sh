#!/bin/bash

avr-gcc -mmcu=atmega328p -std=c99 -Wall -pedantic -Os slider.c stepper.c ../lib/millis.c
