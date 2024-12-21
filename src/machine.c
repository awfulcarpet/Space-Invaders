#include <bits/types/struct_timeval.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "cpu.h"
#include "machine.h"

int
machineIN(struct Machine *machine, uint8_t port) {
	switch (port) {
		case 0:
			return 1;
		case 1:
			return 0;
		case 3:
			return (machine->shift_val >> (8 - machine->shift_offset)) & 0xff;
			break;
		default:
			/*fprintf(stderr, "wrong port read from: %d\n", port);*/
			/*abort();*/
			break;
	}
	return 0;
}

int
machineOUT(struct Machine *machine, uint8_t port) {
	uint8_t a = machine->cpu.a;
	switch (port) {
		case 2:
			machine->shift_offset = a & 0x7;
			break;
		case 4:
			machine->shift_val = (machine->shift_val >> 8) | (a << 8);
			break;
		default:
			/*fprintf(stderr, "wrong port written to: %d\n", port);*/
			/*abort();*/
			break;
	}
	return 0;
}

static double
getmsec(void) {
	struct timeval time;
	gettimeofday(&time, NULL);

	return (time.tv_sec * 1000000) + time.tv_usec;
}

void
run_machine(struct Machine *machine) {
	int cycles = 0;
	machine->last_interrupt = 1;
	double now = getmsec();
	machine->timer = now;
	machine->next_interrupt = now + 16000.0;
	while (1) {
		now = getmsec();

		while (cycles < 17066) {
			unsigned char *op = &machine->cpu.ram[machine->cpu.pc];
			if (*op == 0xdb) { // IN
				printf("inputting %d\n", op[1]);
				machine->cpu.a = machineIN(machine, op[1]);
				machine->cpu.pc += 2;
				cycles += 10;
			} else if (*op == 0xd3) { // OUT
				printf("outputting %d\n", op[1]);
				machineOUT(machine, op[1]);
				machine->cpu.pc += 2;
				cycles += 3;
			} else {
				cycles += emulate(&machine->cpu);
			}
		}

		if (machine->cpu.interrupts == 1 && (now > machine->next_interrupt)) {
			if (machine->last_interrupt == 1) {
				printf("interrupt %d\n", 1);
				generate_interrupt(&machine->cpu, 1);
				machine->last_interrupt = 0;
			} else {
				printf("interrupt %d\n", 2);
				generate_interrupt(&machine->cpu, 2);
				machine->last_interrupt = 1;
			}
			machine->next_interrupt = now + 8000.0;
		}
		cycles = 0;
		machine->timer = now;
		usleep(8000);
	}
}
