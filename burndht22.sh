#!/bin/bash
avr-gcc -mmcu=atmega328p -Wall -Os dht22reader.c lib/dht.c lib/blinker.c && avr-objcopy -j .text -j .data -O ihex a.out a.out.hex && gpio -g write 8 0 && /usr/local/bin/avrdude -p m328p -P /dev/spidev0.0 -c linuxspi -b 40000 -e -U flash:w:a.out.hex && gpio -g write 8 1

