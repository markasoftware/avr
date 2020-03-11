#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main() {
	sei();

	DDRB = 0xff;
	PORTB = 0x00;
	// start timer 0 with /1024 prescaler
	TCCR0B = (1<<CS02) | (1<<CS00);
	// timer interrupt: overflow only
	TIMSK = (1<<TOIE0);

	while (1) {
	}
}

ISR(TIM0_OVF_vect) {
	PORTB ^= 0xff;
}
