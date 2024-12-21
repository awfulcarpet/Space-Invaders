#include <stdlib.h>

#include "cpu.h"
#include "machine.h"

int
machine_init(struct Machine *machine) {
	machine->cpu = calloc(1, sizeof(struct CPU));

	FILE *f = fopen("space-invaders.rom", "r");
	if (map(machine->cpu, f)) return 1;
	fclose(f);

	// link all ports
	for (int i = 0; i < 7; i++) {
		machine->cpu->ports[i] = &machine->ports[i];
	}

	return 0;
}
