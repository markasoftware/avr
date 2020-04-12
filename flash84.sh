#!/bin/bash

src="$1"
echo "Compile..."
avr-gcc -o "${src/.c/.elf}" -O -DF_CPU=8000000 -mmcu=attiny84 "$src"
echo "Flash..."
avrdude -c avrisp -b 19200 -p attiny84 -P /dev/ttyACM? -U flash:w:"${src/.c/.elf}"
