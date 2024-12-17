#include <stdint.h>
#include "raylib.h"

#include "cpu.h"
#include "machine.h"

struct Machine cabinet = {0};

int
main(void) {
	InitWindow(256, 254, "Space Invaders Emulated");
	struct Machine cabinet = {0};
	struct CPU *cpu = &cabinet.cpu;

	cabinet.ports[0] = 0b01110000;
	cabinet.ports[1] = 0b00010000;

	FILE *f = fopen("space-invaders.rom", "r");
	if (map(cpu, f)) return 1;
	fclose(f);


	int run = 0;
	while (!run) {
		if (WindowShouldClose()) {
			break;
		}

		uint8_t *opcode = &cpu->ram[cpu->registers.pc];

		if (opcode[0] == 0xdb) { // IN d8
			cpu->registers.a = machineIN(&cabinet, opcode[1]);
			cpu->registers.pc += 2;
			continue;
		}
		if (opcode[0] == 0xd3) { // OUT d8
			machineOUT(&cabinet, opcode[1]);
			cpu->registers.pc += 2;
			continue;
		}

		run = emulate(cpu);

		BeginDrawing();
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
