#include <avr/io.h>
#include <util/delay.h>

int main() {
	// all pins to output
	DDRB = 0xff;
	while (1) {
		// set them all high
		PORTB = 0xff;
		_delay_ms(250);
		PORTB = 0x00;
		_delay_ms(250);
	}
}
