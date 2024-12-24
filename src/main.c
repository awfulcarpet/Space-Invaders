#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <sys/time.h>
#include <stdint.h>
#include <assert.h>
#include <SDL2/SDL.h>

#include "dissasemble.h"
#include "cpu.h"
#include "machine.h"

extern const int WIDTH;
extern const int HEIGHT;

const int SCREEN_FPS = 60;
const double MS_PER_FRAME = 1000.0 / 60.0;
struct Machine cabinet = {0};

double
getmsec() {
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

	if (win == NULL) {
		fprintf(stderr, "unable to create sdl win: %s\n", SDL_GetError());
		return 1;
	}

	cabinet.framebuffer = SDL_GetWindowSurface(win)->pixels; // destroy window will free the surface for us

	int cycles = 0;
	double timer = getmsec();

	while (1) {
		double dt = getmsec() - timer;
		int cycle_target = dt * 2000; // 2000 cycles per milisecond

		for (cycles = 0; cycles < cycle_target;) {
			cycles += emulate(cabinet.cpu);
			shift_register(&cabinet);
		}

		if (cabinet.cpu->interrupts) {
			generate_interrupt(cabinet.cpu, 2);
		}

		timer = getmsec();

		machine_draw_surface(&cabinet);
		SDL_UpdateWindowSurface(win);

		get_input(&cabinet);
		SDL_Delay(MS_PER_FRAME);
	}

	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}
