#include <stdint.h>
#include <stdio.h>
#include "raylib.h"

#include "cpu.h"
#include "machine.h"

struct Machine cabinet = {0};

int
main(void) {

	InitWindow(256, 254, "Space Invaders Emulated");

	struct Machine cabinet = {0};
	struct CPU *cpu = &cabinet.cpu;

	cabinet.ports[0] = 0b00001110;
	cabinet.ports[1] = 0b00010000;

	/*FILE *f = fopen("space-invaders.rom", "r");*/
	int len = 0;
	cpu->ram = LoadFileData("space-invaders.rom", &len);
	/*if (map(cpu, f)) return 1;*/
	/*fclose(f);*/


	int run = 0;
	while (!run) {
		if (WindowShouldClose()) {
			break;
		}

		setKeys(&cabinet);

		printf("%08b\n", cabinet.ports[0]);
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
		ClearBackground(WHITE);
			DrawText(TextFormat("%02x\n", cpu->ram[0x32]), 0, 0, 32, BLACK);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
