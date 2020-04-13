#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

// how many LSB to add to all ADC readings. To be most accurate, you would want
// to measure sub-LSB offsets, but this isn't a major concern for me. This
// depends on the AVR chip but shouldn't vary much with things like time or
// supply voltage.
#define ADC_CALIBRATE_OFFSET 1

#define R2 5100

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
	seg7(num / 1000 % 10);
	dig1();
	_delay_us(MULTIPLEX_DELAY_US);
	off();
}

int main() {
	// let's go to 8mhz, for fun!
	CLKPR = 0x80;
	CLKPR = 0x00;

	off();

	DDRA = 0b11111100;
	DDRB = 0b00000110;


	// adc accuracy sleep
	MCUCR = 0b00101000;
	PPR = 0b00001110;

	sei();

	// start the ADC in free-running mode
	ADMUX = 0x00; // Vcc as reference, select A0 as input
	ADCSRA = 0b11001111; // enable auto trigger, set /128 adc clock div
	
	while (1) {
		ADCSRA |= (1 << 6);
		sleep_mode();
	}
}

ISR(ADC_vect) {
	unsigned adc = ADC + ADC_CALIBRATE_OFFSET;
	// we don't want to do R2*adc/(1023 - adc) because R2*adc might be a
	// long. but if we do the division first, the rounding happens, so the
	// result could be low by up to the value of adc (which may be a lot!
	// imagine R2=1000 and adc=1001. r1 would be zero, even though our
	// resistor is actually quite large). Luckily, we can add in the
	// remainder later! a/b = a//b + a%b/b, and (a*c)//b = a//b + a%b*c
	// unsigned r1 = R2 / (1023 - adc) * adc + R2 % (1023 - adc) * adc / (1023 - adc);
	unsigned long r1 = (long) R2 * adc / (1023 - adc);

	seg7x4(r1);
}
