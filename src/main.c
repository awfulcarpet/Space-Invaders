#include <stdint.h>
#include <stdlib.h>
#include "cpu.h"

int
main(void) {
	struct CPU cpu = {0};
	cpu.ram = calloc(4000, sizeof(uint8_t));

	while (!emulate(&cpu));

	return 0;
}
