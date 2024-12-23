#include <SDL2/SDL_error.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <sys/time.h>
#include <stdint.h>
#include <assert.h>
#include <SDL2/SDL.h>

#include "dissasemble.h"
#include "cpu.h"
#include "machine.h"

const int WIDTH = 224;
const int HEIGHT = 256;
struct Machine cabinet = {0};

double
getsec() {
	struct timeval time;
	gettimeofday(&time, NULL);
	return (double)time.tv_sec * 1000 + (time.tv_usec/1000.0);
}

int
main(int argc, char **argv) {
	assert(machine_init(&cabinet) == 0);

	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Space Invaders", 0, 0, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Surface *surf = NULL;

	if (win == NULL) {
		fprintf(stderr, "unable to create sdl win: %s\n", SDL_GetError());
		return 1;
	}

	surf = SDL_GetWindowSurface(win);
	SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 0xFF, 0xff, 0xff));
	SDL_UpdateWindowSurface(win);
	SDL_Event e; bool quit = false; while (quit == false) { while (SDL_PollEvent(&e) ) { if (e.type == SDL_QUIT) quit = true; }}

	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
	int cycles = 0;

	print_cpu_state(cabinet.cpu, cycles);
	double last_interrupt = 0;
	while (1) {
		get_opname(cabinet.cpu->ram, cabinet.cpu->pc);
		cycles += emulate(cabinet.cpu);
		shift_register(&cabinet);
		print_cpu_state(cabinet.cpu, cycles);

		if (cabinet.cpu->interrupts && (getsec() - last_interrupt) > 1.0/60.0) {
			generate_interrupt(cabinet.cpu, 2);
			last_interrupt = getsec();
		}
	}

	return 0;
}
