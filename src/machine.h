#include <stdint.h>
struct Machine {
	struct CPU *cpu;

	uint8_t iports[4];
	uint8_t oports[7];
	uint16_t shift;
	uint8_t offset;
	void *video; // implement later
};

int machine_init(struct Machine *machine);

void shift_register(struct Machine *machine);
void print_shift(struct Machine *machine);
