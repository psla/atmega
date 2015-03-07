#!/bin/bash
codefile=$1

if [ "$1" == "" ]
then
  echo "File not specified"
fi

if [ ! -f $1 ] 
then
  echo "file $1 does not exist"
  exit
fi

avr-gcc -mmcu=atmega328p -Wall -Os -o $1.elf $1
avr-objcopy -j .text -j .data -O ihex $1.elf $1.hex
gpio -g write 8 0
/usr/local/bin/avrdude -p m328p -P /dev/spidev0.0 -c linuxspi -b 10000 -e -U flash:w:$1.hex
gpio -g write 8 1
