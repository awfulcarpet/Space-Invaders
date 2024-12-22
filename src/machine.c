#include <stdint.h>
#include <stdlib.h>

#include "cpu.h"
#include "machine.h"

int
machine_init(struct Machine *machine) {
	machine->cpu = calloc(1, sizeof(struct CPU));

	FILE *f = fopen("space-invaders.rom", "r");
	if (map(machine->cpu, f)) return 1;
	fclose(f);

	for (int i = 0; i <= 3; i++) {
		machine->cpu->iports[i] = &(machine->iports[i]);
	}
	for (int i = 2; i <= 6; i++) {
		machine->cpu->oports[i] = &(machine->oports[i]);
	}

	return 0;
}

void
shift_register(struct Machine *machine) {

	machine->offset = (machine->oports[2] & 0b111);

	if (machine->cpu->shift_written == 0)
		return;

	machine->shift = (machine->oports[4] << 8) | (machine->shift >> 8);

	machine->iports[3] = (machine->shift & (0xff00 >> machine->offset)) >> (8 - machine->offset);

	machine->cpu->shift_written = 0;
}

void
print_shift(struct Machine *machine) {
	printf("offset: %d ", machine->offset);
	printf("reg: %016b ", machine->shift);
	printf("ret: %08b ",  (machine->shift & (0xff00 >> machine->offset)) >> (8 - machine->offset));
	printf("\n");
}
