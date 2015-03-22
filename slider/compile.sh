#!/bin/bash

avr-gcc -DF_CPU=8000000L -mmcu=atmega328p -std=c99 -Wall -pedantic -Os slider.c stepper.c ../lib/millis.c
