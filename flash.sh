#!/bin/bash

src="$1"
echo "Compile..."
avr-gcc -Os -DF_CPU=1000000 -mmcu=attiny85 -c "$src"
echo "Link? ..."
avr-gcc -o "${src/.c/.elf}" -DF_CPU=1000000 -mmcu=attiny85 "${src/.c/.o}"
echo "Flash..."
avrdude -c avrisp -b 19200 -p attiny85 -P /dev/ttyACM0 -U flash:w:"${src/.c/.elf}"
