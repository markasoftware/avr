#!/bin/bash

src="$1"
echo "Compile..."
avr-gcc -o "${src/.c/.elf}" -O -DF_CPU=1000000 -mmcu=attiny85 "$src"
echo "Flash..."
avrdude -c avrisp -b 19200 -p attiny85 -P /dev/ttyACM? -U flash:w:"${src/.c/.elf}"
