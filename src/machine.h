#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>

struct Machine {
	struct CPU cpu;

	double last_interrupt;
	double next_interrupt;
	double interrupt;
	double timer;

	/*uint8_t ports[7];*/
	uint8_t shift_offset;
	uint8_t shift0;
	uint8_t shift1;
	uint16_t shift_val;
};

int machineIN(struct Machine *machine, uint8_t port);
int machineOUT(struct Machine *machine, uint8_t port);
void draw_display(struct Machine *machine, SDL_Surface *surface, SDL_Window *window);
void run_machine(struct Machine *machine, SDL_Surface * surface, SDL_Window *window);
