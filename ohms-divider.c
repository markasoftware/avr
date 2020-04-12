#include <avr/io.h>

static void seg7(unsigned char num) {
	PORTA &= (0b00111100);
	PORTA |= (num << 2);
}

int main() {
	// default LEDs to off.
	PORTA = 0xff;
	PORTB = 0xff;

	DDRA = 0b11111100;
	DDRB = 0b00000110;

	// Turn on some segment
	PORTB &= ~(1 << PORTB1);
}
