#include <avr/io.h>
#include <util/delay.h>

static void seg7(unsigned char num) {
	PORTA |= (num << 2);
}

static void off() {
	PORTB = PORTB | (1 << PORTB1) | (1<<PORTB2);
	PORTA = PORTA | (1<<PORTA6) | (1<<PORTA7);
	PORTA &= ~0b00111100;
}

#define DEFINE_DIGIT(digit, port, pin) \
	static void dig ## digit () {    \
		port &= ~(1 << pin);	 \
	}

DEFINE_DIGIT(1, PORTB, PORTB1)
DEFINE_DIGIT(2, PORTB, PORTB2)
DEFINE_DIGIT(3, PORTA, PORTA7)
DEFINE_DIGIT(4, PORTA, PORTA6)

#define MULTIPLEX_DELAY_US 100 

static void seg7x4(unsigned num) {
	seg7(num % 10);
	dig4();
	_delay_us(MULTIPLEX_DELAY_US);
	off();
	seg7(num / 10 % 10);
	dig3();
	_delay_us(MULTIPLEX_DELAY_US);
	off();
	seg7(num / 100 % 10);
	dig2();
	_delay_us(MULTIPLEX_DELAY_US);
	off();
	seg7(num / 1000);
	dig1();
	_delay_us(MULTIPLEX_DELAY_US);
	off();
}

int main() {
	// let's go to 8mhz, for fun!
	CLKPR = 0x80;
	CLKPR = 0x00;

	// default LEDs to off.
	PORTA = 0xff;
	PORTB = 0xff;

	DDRA = 0b11111100;
	DDRB = 0b00000110;

	off();

	unsigned k = 0;
	for (unsigned i = 0; i < 100000; i++) {
		if (i % 100 == 0) {
			k++;
		}
		seg7x4(k);
	}
}
