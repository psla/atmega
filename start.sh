#!/bin/bash

# 
# Raspberry Pi Atmega programmer.
# Pinout:
# RPI || Atmega 328P-PU
# 24  || 1 (reset)
# MOSI	19 || 17 
# MISO	21 || 18
# SCLK	23 || 19
# 3.3V  17 || 7, 20
# GND	25 || 8, 22

#
# Raspbmc is somehow broken with SPI
#
# Initialize SPI by the trick of running boblightd
boblightd --help > /dev/null

# GPIO 8 (port 24) is used to control reset pin.
# connect it to ATMEGA 328P-PU port 1
#
gpio -g mode 8 out
gpio -g write 8 0

# now, this should report success:
avrdude -p m328p -P /dev/spidev0.0 -c linuxspi -b 10000

# set 8 to 0 to program
# set 8 to 1 to start your application
