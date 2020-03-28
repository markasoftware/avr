// simavr tester for blink.c
// compile: gcc -lsimavr -lelf blink-test.c

#include <stdio.h>
#include <stdlib.h>

#include <simavr/sim_avr.h>
#include <simavr/sim_elf.h>
#include <simavr/avr_ioport.h>

void pin_change_hook(struct avr_irq_t *irq, uint32_t value, void *param) {
	if (value == 1) {
		puts("Wax on...");
	} else {
		puts("Wax off!");
	}
}

int main() {
	avr_t *avr;
	elf_firmware_t firmware;

	elf_read_firmware("blink.elf", &firmware);

	printf("About to simulate at frequency %d Hz\n", firmware.frequency);

	if (!(avr = avr_make_mcu_by_name("attiny85"))) {
		fprintf(stderr, "Could not create AVR object\n");
		exit(1);
	}

	avr_init(avr);
	avr_load_firmware(avr, &firmware);

	// bit 0 of PORTB, basically
	avr_irq_t *portb_0_irq = avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 0);
	avr_irq_register_notify(portb_0_irq, &pin_change_hook, NULL);

	const struct timespec one_microsecond = {
		.tv_sec = 0,
		.tv_nsec = 1000,
	};
	while (1) {
		avr_run(avr);
		// lol. Plz compile without optimization.
		// (yes, this works. In gcc)
		for(int i = 0; i < 1000; i++);
	}
}
