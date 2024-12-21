#include <SDL2/SDL.h>
#include <bits/types/struct_timeval.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
/*#include "raylib.h"*/

#include "cpu.h"
#include "machine.h"

struct Machine cabinet = {0};
SDL_Surface *surface;
SDL_Window *window;

int
main(void) {

	/*InitWindow(256, 224, "Space Invaders Emulated");*/
	/*SetTargetFPS(60);*/

	struct Machine cabinet = {0};
	struct CPU *cpu = &cabinet.cpu;

	FILE *f = fopen("space-invaders.rom", "r");
	/*int len = 0;*/
	/*cpu->ram = LoadFileData("space-invaders.rom", &len);*/
    cpu->ram = malloc(0x16384);
    fseek(f, 0, SEEK_END);
    int fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);
	fread(cpu->ram, fsize, 1, f);
	/*if (map(cpu, f)) return 1;*/
	fclose(f);

	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Space Invaders Emulator",
							SDL_WINDOWPOS_CENTERED,
							SDL_WINDOWPOS_CENTERED,
							224, 256,
							SDL_WINDOW_SHOWN);

        surface = SDL_GetWindowSurface(window);
	run_machine(&cabinet, surface, window);
	/*CloseWindow();*/
	return 0;
}
