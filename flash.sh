#!/bin/bash

src="$1"
tiny_model=${2:-85}
echo "Compile..."
avr-gcc -o "${src/.c/.elf}" -Os -DF_CPU=1000000 -mmcu=attiny85 "$src"
echo "Flash..."
avrdude -c avrisp -b 19200 -p attiny$tiny_model -P /dev/ttyACM? -U flash:w:"${src/.c/.elf}"
