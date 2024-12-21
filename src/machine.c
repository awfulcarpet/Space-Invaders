#include <stdint.h>
#include <stdlib.h>
#include <raylib.h>
#include <string.h>

#include "cpu.h"
#include "machine.h"

int
machineIN(struct Machine *machine, uint8_t port) {
	switch (port) {
		case 3:
			return machine->shift_val & (0xff << (15 - machine->shift_offset));
			break;
		default:
			fprintf(stderr, "wrong port read from: %d\n", port);
			/*abort();*/
			break;
	}
	return 0;
}

int
machineOUT(struct Machine *machine, uint8_t port) {
	uint8_t a = machine->cpu.registers.a;
	switch (port) {
		case 2:
			machine->shift_offset = a & 0x7;
			break;
		case 3: // TODO: Sounds
			break;
		case 4:
			machine->shift_val = (machine->shift_val >> 8) | (a << 8);
			break;
		case 6:
			machine->ports[port] = 1;
			break;
		default:
			fprintf(stderr, "wrong port written to: %d\n", port);
			/*abort();*/
			break;
	}
	return 0;
}

void
draw_display(struct Machine *machine) {
	int width = 256;
	int height = 224;

	unsigned char *buffer = calloc(width * height, sizeof(unsigned char));
	unsigned char *framebuffer = &machine->cpu.ram[2400];

	// memory has it flipped 90 degrees clockwise
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j += 8) {
			unsigned char line = framebuffer[i * (height/8) + (j/8)];

			for (int p = 0; p < 8; p++) {
				buffer[(height - 1 - p - j) * width + i] = (line & (0x1 << (7 - p))) > 0;
			}
		}
	}
	/*for (int i = 0; i < 224; i++) {*/
	/*	for (int j = 0; j < 256; j += 8) {*/
	/*		int p;*/
	/*		unsigned char pi = framebuffer[(i*(256/8)) + j/8];*/
	/**/
	/*		int offset = (255-j)*(224*4) + (i*4);*/
	/*		unsigned int *p1 = (unsigned int *)(&buffer[offset]);*/
	/**/
	/*		for (p = 0; p < 8; p++) {*/
	/*			if (0 != (pi & (1 << p)))*/
	/*				*p1 = 0xFFFFFFFFL;*/
	/*			else*/
	/*				*p1 = 0x00000000L;*/
	/*			p1-=224;*/
	/*		}*/
	/*	}*/
	/*}*/

	Image disp = {
		.data = buffer,
		.width = width,
		.height = height,
		.format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
		/*.format = PIXELFORMAT_UNCOMPRESSED_R32,*/
	};

	Texture2D texture = LoadTextureFromImage(disp);
	DrawTexture(texture, 0, 0, WHITE);

	UnloadTexture(texture);
	free(buffer);
}

void
setKeys(struct Machine *machine) {
	machine->ports[0] |= IsKeyDown(KEY_SPACE) << 4;
	machine->ports[0] |= IsKeyDown(KEY_LEFT) << 5;
	machine->ports[0] |= IsKeyDown(KEY_RIGHT) << 6;

	machine->ports[0] &= ~(IsKeyUp(KEY_SPACE) << 4);
	machine->ports[0] &= ~(IsKeyUp(KEY_LEFT) << 5);
	machine->ports[0] &= ~(IsKeyUp(KEY_RIGHT) << 6);
}
