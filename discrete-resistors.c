#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>

#include "discrete-constants.h"

// how many LSB to add to all ADC readings. To be most accurate, you would want
// to measure sub-LSB offsets, but this isn't a major concern for me. This
// depends on the AVR chip but shouldn't vary much with things like time or
// supply voltage.
#define ADC_CALIBRATE_OFFSET 1

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
	PRR = 0b00001110;

	sei();

	// start the ADC in free-running mode
	ADMUX = 0x00; // Vcc as reference, select A0 as input
	ADCSRA = 0b11001111; // enable auto trigger, set /128 adc clock div
	
	while (1) {
		ADCSRA |= (1 << 6);
		sleep_mode();
	}
}

static void recount(unsigned long g_total, int *r1_n, int *r2_n) {
	*r1_n = G1_GCD_COEF * g_total;
	*r2_n = G2_GCD_COEF * g_total;
	int how_many_lcms;
	if (*r1_n < 0) {
		how_many_lcms = -*r1_n / G1_LCM_COEF;
		if (how_many_lcms * G1_LCM_COEF < -*r1_n) {
			how_many_lcms++;
		}
		*r1_n += how_many_lcms * G1_LCM_COEF;
		*r2_n -= how_many_lcms * G2_LCM_COEF;
	} else if (*r2_n < 0) {
		how_many_lcms = -*r2_n / G2_LCM_COEF;
		if (how_many_lcms * G2_LCM_COEF < -*r2_n) {
			how_many_lcms++;
		}
		*r1_n -= how_many_lcms * G1_LCM_COEF;
		*r2_n += how_many_lcms * G2_LCM_COEF;
	}
}

ISR(ADC_vect) {
	unsigned adc = ADC + ADC_CALIBRATE_OFFSET;
	// it's crucially important to round this to the *nearest*, not down.
	unsigned long g_total = ((unsigned long) (1023 - adc) * G_MULTIPLIER + ((unsigned long)R_DIVIDER * adc / 2))
		/ ((unsigned long)R_DIVIDER * adc);

	int r1_n, r2_n;
	for (char i = 1; i < 20; i++) {
		recount(g_total, &r1_n, &r2_n);
		if (r1_n + r2_n < 10 && r1_n >= 0 && r2_n >= 0) {
			seg7x4(r2_n * 100 + r1_n);
			break;
		}
		g_total += i % 2 ? i : -i;
	}

}
