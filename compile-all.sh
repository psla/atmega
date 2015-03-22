#!/bin/bash
set -e

#
# Compiles all mini-projects.
# When new script is added, make sure to add it here
#
# Main purpose of this script is to enable continuous integration (well, at least continuous builds :) ) to make 
# sure that no regression happened for other projects, when refactoring 

if [ -d output ]
then
  rm -rf output
fi
mkdir output

avr-gcc -mmcu=atmega328p -Wall -Os -o output/blink1.elf blink1.c
avr-objcopy -j .text -j .data -O ihex output/blink1.elf output/blink1.hex

# dht
avr-gcc -mmcu=atmega328p -Wall -Os dhtreader.c lib/dht.c lib/blinker.c

# dht 22
avr-gcc -mmcu=atmega328p -Wall -Os dht22reader.c lib/dht.c lib/blinker.c

# RFM
# avr-g++  -mmcu=atmega328p -Wall -Os radio.cpp lib/RFM69.cpp lib/spi.c lib/common.c lib/blinker.c

cd slider
./compile.sh
