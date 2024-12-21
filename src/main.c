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

	FILE *f = fopen("space-invaders.rom", "r");
	/*int len = 0;*/
	/*cpu->ram = LoadFileData("space-invaders.rom", &len);*/
	if (map(cpu, f)) return 1;
	fclose(f);

			run_machine(&cabinet);
	return 0;
}
