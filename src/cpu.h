#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

enum FLAGS {
	CARRY = 0x01,
	PARITY = 0x01 << 2,
	ZERO = 0x01 << 6,
	SIGN = 0x01 << 7,
	ALL = CARRY | PARITY | ZERO | SIGN,
};

struct Flags {
	uint8_t c:1;
	uint8_t v:1;
	uint8_t p:1;
	uint8_t pad:1;
	uint8_t a:1;
	uint8_t k:1;
	uint8_t z:1;
	uint8_t s:1;
};

struct Registers {
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t h;
	uint8_t l;
	uint16_t sp;
	uint16_t pc;
};

struct CPU {
	struct Flags flags;
	struct Registers registers;
	uint8_t *ram; // little endian
	bool interrupts;
};

int map(struct CPU *cpu, FILE *f);
void generate_interrupt(struct CPU *cpu, int interrupt);
int emulate(struct CPU *cpu);
void print_cpu_state(struct CPU *cpu);
