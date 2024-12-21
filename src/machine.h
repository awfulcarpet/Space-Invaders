#include <stdint.h>
struct Machine {
	struct CPU *cpu;

	uint8_t ports[7];
	void *video; // implement later
};

int machine_init(struct Machine *machine);
