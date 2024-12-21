#include <stdint.h>
#include <assert.h>
#include "cpu.h"
#include "machine.h"

struct Machine cabinet = {0};

int
main(void) {

	assert(machine_init(&cabinet) == 0);

	int cycles = 0;
	while (1) {
		cycles += emulate(cabinet.cpu);
		print_cpu_state(cabinet.cpu, cycles);
	}

	return 0;
}
