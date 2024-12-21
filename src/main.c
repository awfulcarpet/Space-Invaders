#include <bits/types/struct_timeval.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include "raylib.h"

#include "cpu.h"
#include "machine.h"

struct Machine cabinet = {0};

int
main(void) {

	/*InitWindow(256, 224, "Space Invaders Emulated");*/
	/*SetTargetFPS(60);*/

	struct Machine cabinet = {0};
	struct CPU *cpu = &cabinet.cpu;

	/*cabinet.ports[0] = 0b00001110;*/
	/*cabinet.ports[1] = 0b00010000;*/

	FILE *f = fopen("space-invaders.rom", "r");
	/*int len = 0;*/
	/*cpu->ram = LoadFileData("space-invaders.rom", &len);*/
	if (map(cpu, f)) return 1;
	fclose(f);


	int run = 0;

	while (1) {
		emulate(cpu);
	}
	/*while (!run) {*/
		/*if (WindowShouldClose()) {*/
		/*	break;*/
		/*}*/
		/**/
		/*double now = get_time();*/
		/**/
		/*if (cabinet.timer == 0.0) {*/
		/*	cabinet.timer = now;*/
		/*	cabinet.next_interrupt = cabinet.timer + 16000.0;*/
		/*	cabinet.interrupt = 1;*/
		/*}*/
		/**/
		/*if (cpu->interrupts && now > cabinet.next_interrupt) {*/
		/*	if (cabinet.next_interrupt == 1) {*/
		/*		generate_interrupt(cpu, 1);*/
		/*		cabinet.interrupt = 2;*/
		/*	} else {*/
		/*		generate_interrupt(cpu, 2);*/
		/*		cabinet.interrupt = 1;*/
		/*	}*/
		/*	cabinet.next_interrupt = now + 8000.0;*/
		/*}*/
		/**/
		/**/
		/*double delta = get_time() - cabinet.timer;*/
		/*int left = delta * 2;*/
		/*int cycles = 0;*/
		/**/
		/**/
		/*while (cycles < left) {*/
		/*	uint8_t *opcode = &cpu->ram[cpu->registers.pc];*/
		/**/
		/*	if (opcode[0] == 0xdb) { // IN d8*/
		/*		cpu->registers.a = machineIN(&cabinet, opcode[1]);*/
		/*		cpu->registers.pc += 2;*/
		/*		cycles += 3;*/
		/*	}*/
		/*	if (opcode[0] == 0xd3) { // OUT d8*/
		/*		machineOUT(&cabinet, opcode[1]);*/
		/*		cpu->registers.pc += 2;*/
		/*		cycles += 3;*/
		/*	}*/
		/**/
			/*cycles += emulate(cpu);*/
			/*emulate(cpu);*/
		/*getchar();*/
		/*}*/

		/*cabinet.timer = get_time();*/

		/*BeginDrawing();*/
		/*ClearBackground(WHITE);*/
		/*	setKeys(&cabinet);*/
		/*	draw_display(&cabinet);*/
		/*EndDrawing();*/
	/*}*/

	/*CloseWindow();*/
	return 0;
}
