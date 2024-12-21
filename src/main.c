#include <stdint.h>
#include "cpu.h"

int
main(void) {
	struct CPU cpu = {0};

	FILE *f = fopen("space-invaders.rom", "r");
	if (map(&cpu, f)) return 1;
	fclose(f);

	int cycles = 0;
	while (1) {
		cycles += emulate(&cpu);
		print_cpu_state(&cpu, cycles);
	}

	return 0;
}
