#include <SDL2/SDL_events.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "cpu.h"
#include "machine.h"

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
		case SDL_QUIT:
			exit(1);
		break;
	}
}

void
machine_draw_surface(struct Machine *machine) {
	uint32_t *pixel = machine->framebuffer;
	for (int x = 0; x < 224; x++) {
		pixel[10 * 224 + x] = 0xFFFFFF;
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
