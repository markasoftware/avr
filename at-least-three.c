/**
 * Attach buttons, that connect to ground, on pins 0-3. LED on 4.
 *
 * Only turns on the light when at least three buttons/switches are on.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>
#include <avr/sleep.h>

// fun test: These aren't quite inlined if you don't put static
static void light_on() {
	PORTB |= (1<<PORTB4);
}

static void light_off() {
	PORTB &= ~(1<<PORTB4);
}

static void update_light() {
	char buttons_depressed = 0;
	for (char i = 0; i <= 3; i++) {
		if ((PINB & (1<<i)) == 0) buttons_depressed++;
	}

	if (buttons_depressed >= 3) {
		light_on();
	} else {
		light_off();
	}

}

int main() {
	DDRB = 0b00010000;
	// enable pull-up resistors
	PORTB = 0b00001111;
	// enable interrupts on pins 0-3
	sei();
	// enable pin interrupts in general
	GIMSK = (1<<PCIE);
	// enable interrupts on our specific pins
	PCMSK = 0b00001111;

	// wait for pullups to take effect, though they probably have already
	_NOP();
	update_light();

	// set bits for power-down sleep (pin interrupts still work)
	// SE means basic sleep enable, SM1 indicates powerdown.
	// MCUCR = (1<<SE) | (1<<SM1);
	// But we can let the AVR libraries do it for us. This sets SM1:
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	while (1) {
		// And this sets SE, sends the sleep instruction, then clears
		// SE:
		sleep_mode();
		// sleep_cpu() issues the sleep instruction without touching SE.
	}
}

ISR(PCINT0_vect) {
	update_light();
}
