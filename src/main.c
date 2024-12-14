#include <stdint.h>
#include "cpu.h"

int
main(void) {
	struct CPU cpu;
	cpu.registers.pc = 0;

	FILE *f = fopen("space-invaders.rom", "r");
	if (map(&cpu, f)) return 1;
	fclose(f);

	print_cpu_state(&cpu);
	while (!emulate(&cpu));

	return 0;
}
