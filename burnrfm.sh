#!/bin/bash
set -e
avr-g++  -mmcu=atmega328p -Wall -Os radio.cpp lib/RFM69.cpp  lib/millis.c lib/spi.c lib/common.c
avr-objcopy -j .text -j .data -O ihex a.out a.out.hex && gpio -g write 8 0 && /usr/local/bin/avrdude -p m328p -P /dev/spidev0.0 -c linuxspi -b 10000 -e -U flash:w:a.out.hex && gpio -g write 8 1

