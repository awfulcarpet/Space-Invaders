#include <stdint.h>

struct Machine {
	struct CPU cpu;
	uint8_t ports[7];
};

int machineIN(struct Machine *machine, uint8_t port);
int machineOUT(struct Machine *machine, uint8_t port);
