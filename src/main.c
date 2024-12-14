#include <stdint.h>
#include "cpu.h"

int
main(void) {
	struct CPU cpu = {0};

	FILE *f = fopen("space-invaders.rom", "r");
	if (map(&cpu, f)) return 1;
	fclose(f);

	while (!emulate(&cpu));

	return 0;
}
