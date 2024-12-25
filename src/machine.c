#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "cpu.h"
#include "machine.h"

const int WIDTH = 224;
const int HEIGHT = 256;

int
machine_init(struct Machine *machine) {
	machine->cpu = calloc(1, sizeof(struct CPU));

	FILE *f = fopen("space-invaders.rom", "r");
	if (map(machine->cpu, f)) return 1;
	fclose(f);

	for (int i = 0; i <= 3; i++) {
		machine->cpu->iports[i] = &(machine->iports[i]);
	}
	for (int i = 2; i <= 6; i++) {
		machine->cpu->oports[i] = &(machine->oports[i]);
	}

	return 0;
}

void get_input(struct Machine *machine) {
	SDL_Event e;
	SDL_PollEvent(&e);
	switch (e.type) {
		case SDL_KEYDOWN:
		{
				switch (e.key.keysym.sym) {
					case SDLK_c: // coin
						machine->iports[1] |= (0x1 << 0);
						break;
					case SDLK_SPACE:
						machine->iports[1] |= (0x1 << 4);
						break;
					case SDLK_LEFT:
						machine->iports[1] |= (0x1 << 5);
						break;
					case SDLK_RIGHT:
						machine->iports[1] |= (0x1 << 6);
						break;
				}
				break;
		}
		case SDL_KEYUP:
		{
				switch (e.key.keysym.sym) {
					case SDLK_c: // coin
						machine->iports[1] &= ~(0x1 << 0);
						break;
					case SDLK_SPACE:
						machine->iports[1] &= ~(0x1 << 4);
						break;
					case SDLK_LEFT:
						machine->iports[1] &= ~(0x1 << 5);
						break;
					case SDLK_RIGHT:
						machine->iports[1] &= ~(0x1 << 6);
						break;
				}
				break;
		}
		case SDL_QUIT:
			exit(1);
		break;
	}
}

void
machine_draw_surface(struct Machine *machine) {
	uint32_t *pixel = machine->framebuffer;
	uint8_t *fb = &machine->cpu->ram[0x2400];

	for (int col = 0; col < WIDTH; col++) {
		for (int line = 0; line < HEIGHT; line += 8) {
			uint8_t chunk = fb[col * (HEIGHT/8) + (line/8)];

			for (int p = 0; p < 8; p++) {
				uint16_t idx = (HEIGHT - 1 - line - p) * WIDTH + col;

				if (chunk & (0x1 << p))
					pixel[idx] = 0xFFFFFF;
				else
					pixel[idx] = 0x000000;
			}
		}
	}
}

void
shift_register(struct Machine *machine) {

	machine->offset = (machine->oports[2] & 0b111);

	if (machine->cpu->shift_written == 0)
		return;

	machine->shift = (machine->oports[4] << 8) | (machine->shift >> 8);

	machine->iports[3] = (machine->shift & (0xff00 >> machine->offset)) >> (8 - machine->offset);

	machine->cpu->shift_written = 0;
}

void
print_shift(struct Machine *machine) {
	printf("offset: %d ", machine->offset);
	printf("reg: %016b ", machine->shift);
	printf("ret: %08b ",  (machine->shift & (0xff00 >> machine->offset)) >> (8 - machine->offset));
	printf("\n");
}
