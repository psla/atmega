#!/bin/bash

avr-gcc -mmcu=atmega328p -Wall -Os slider.c stepper.c
