#include <stdint.h>
#include <stdlib.h>
#include <raylib.h>

#include "cpu.h"
#include "machine.h"

int
machineIN(struct Machine *machine, uint8_t port) {
	switch (port) {
		case 3:
			return machine->shift_val & (0xff << (15 - machine->shift_offset));
			break;
		default:
			fprintf(stderr, "wrong port read from: %d\n", port);
			abort();
			break;
	}
	return 0;
}

int
machineOUT(struct Machine *machine, uint8_t port) {
	uint8_t a = machine->cpu.registers.a;
	switch (port) {
		case 2:
			machine->shift_offset = a & 0x7;
			break;
		case 3: // TODO: Sounds
			break;
		case 4:
			machine->shift_val = (machine->shift_val >> 8) | (a << 8);
			break;
		case 6:
			machine->ports[port] = 1;
			break;
		default:
			fprintf(stderr, "wrong port written to: %d\n", port);
			abort();
			break;
	}
	return 0;
}

void
setKeys(struct Machine *machine) {
	machine->ports[0] |= IsKeyDown(KEY_SPACE) << 4;
	machine->ports[0] |= IsKeyDown(KEY_LEFT) << 5;
	machine->ports[0] |= IsKeyDown(KEY_RIGHT) << 6;

	machine->ports[0] &= ~(IsKeyUp(KEY_SPACE) << 4);
	machine->ports[0] &= ~(IsKeyUp(KEY_LEFT) << 5);
	machine->ports[0] &= ~(IsKeyUp(KEY_RIGHT) << 6);
}
