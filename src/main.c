#include <sys/time.h>
#include <stdint.h>
#include <assert.h>
#include "cpu.h"
#include "machine.h"

struct Machine cabinet = {0};

double
getsec() {
	struct timeval time;
	gettimeofday(&time, NULL);
	return (double)time.tv_sec * 1000 + (time.tv_usec/1000.0);
}

int
main(void) {

	assert(machine_init(&cabinet) == 0);

	int cycles = 0;

	print_cpu_state(cabinet.cpu, cycles);
	double last_interrupt = 0;
	while (1) {
		cycles += emulate(cabinet.cpu);
		shift_register(&cabinet);
		print_cpu_state(cabinet.cpu, cycles);

		if (cabinet.cpu->interrupts && (getsec() - last_interrupt) > 1.0/60.0) {
			generate_interrupt(cabinet.cpu, 2);
			last_interrupt = getsec();
		}
	}


	return 0;
}
