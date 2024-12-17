#include "cpu.h"
#include "machine.h"

int
machineIN(struct Machine *machine, uint8_t port) {
	printf("%02x\n", machine->ports[0]);
	return 0;
}

int
machineOUT(struct Machine *machine, uint8_t port) {
	return 0;
}
