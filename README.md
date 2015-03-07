# atmega
Playing around with microcontrollers

## Raspberry PI
I am using RaspBMC

```
Linux raspbmc 3.12.31 #2 PREEMPT Wed Oct 29 09:24:56 UTC 2014 armv6l GNU/Linux
```

Avrdude from RaspBMC is not supporting LinuxSPI driver.

Compile it from source
```
apt-get install bison autoconf make gcc flex
git clone https://github.com/kcuzner/avrdude.git
cd avrdude/.
./bootstrap
./configure
# make sure linuxspi is ENABLED
make
sudo make install
```

## Workflow
run ./start.sh to initialize programming (set gpio, etc.)
run ./burn-and-run.sh filename.c to compile and burn to atmega
