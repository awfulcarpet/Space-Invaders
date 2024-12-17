#include <stdint.h>

struct Machine {
	struct CPU cpu;
	uint8_t ports[7];
	uint8_t shift_offset;
	uint16_t shift_val;
};

int machineIN(struct Machine *machine, uint8_t port);
int machineOUT(struct Machine *machine, uint8_t port);
void setKeys(struct Machine *machine);
