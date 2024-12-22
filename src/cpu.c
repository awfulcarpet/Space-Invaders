#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "dissasemble.h"

uint8_t
in(struct CPU *cpu, uint8_t port) {
	return *cpu->iports[port];
}

void
out(struct CPU *cpu, uint8_t port) {
	*cpu->oports[port] = cpu->a;

	if (port == 4) {
		cpu->shift_written = 1;
	}
}

int
map(struct CPU *cpu, FILE *f) {
	if (f == NULL) {
		perror("failed to open rom");
		return 1;
	}

	fseek(f, 0, SEEK_END);
	int len = ftell(f);
	fseek(f, 0, SEEK_SET);
	cpu->ram = calloc(len, sizeof(uint8_t));

	fread(cpu->ram, sizeof(uint8_t), len, f);
	return 0;
}

static void
unimplemented(uint8_t opcode) {
	fprintf(stderr, "unimplemented: %02x\n", opcode);
	exit(1);
}

static void
push(struct CPU *cpu, uint8_t high, uint8_t low) {
	cpu->sp -= 2;
	cpu->ram[cpu->sp + 1] = high;
	cpu->ram[cpu->sp] = low;
}

static uint16_t
pop(struct CPU *cpu) {
	uint16_t ret = (cpu->ram[cpu->sp + 1] << 8) | cpu->ram[cpu->sp];
	cpu->sp += 2;
	return ret;
}

static void
call(struct CPU *cpu, uint8_t *opcode) {
	uint16_t adr = cpu->pc + 2;
	push(cpu, adr >> 8, adr & 0xff);
	cpu->pc = (opcode[2] << 8) | opcode[1];
}

static int
parity(int x, int size) {
    uint8_t num = 0;

    for (int i = 0; i < size; i++)
    {
        num += ((x >> i) & 1);
    }

    return (num & 0x1) == 0; // even
}

static void
flagsZSP(struct CPU *cpu, uint8_t num) {
	cpu->flags.z = (num == 0);
	cpu->flags.s = ((num & 0x80) == 0x80);
	/*cpu->flags.p = parity(num, 8);*/
	cpu->flags.p = (num % 2 == 0); // TODO: fix?
}

static uint8_t
get_psw(struct Flags *flags) {
	uint8_t psw = 0;
	psw |= flags->s << 7;
	psw |= flags->z << 6;
	psw |= flags->k << 5;
	psw |= 0 << 4;
	psw |= 0 << 3;
	psw |= flags->p << 2;
	/*psw |= 1 << 1;*/
	psw |= flags->c << 0;
	return psw;
}

static void
set_psw(struct Flags *flags, uint8_t psw) {
	flags->s = (psw >> 7) & 0x1;
	flags->z = (psw >> 6) & 0x1;
	flags->p = (psw >> 2) & 0x1;
	flags->c = psw & 0x1;
}

unsigned char cycles8080[] = {
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x00..0x0f
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4, //0x10..0x1f
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4, //etc
	4, 10, 13, 5, 10, 10, 10, 4, 4, 10, 13, 5, 5, 5, 7, 4,

	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5, //0x40..0x4f
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,

	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4, //0x80..8x4f
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,

	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11, //0xc0..0xcf
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 10, 10, 17, 7, 11,
	11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 5, 17, 17, 7, 11,
	11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 17, 7, 11,
};

int
emulate(struct CPU *cpu) {
	uint8_t *opcode = &cpu->ram[cpu->pc];

	get_opname(cpu->ram, cpu->pc);

	cpu->pc++;
	switch (*opcode) {
		case 0x00: // NOP

			break;
		case 0x01: // LXI  B,d16
			cpu->c = opcode[1];
			cpu->b = opcode[2];
			cpu->pc += 2;

			break;
		case 0x02: // STAX B
			unimplemented(opcode[0]);
			break;
		case 0x03: // INX  B
			unimplemented(opcode[0]);
			break;
		case 0x04: // INR  B
			unimplemented(opcode[0]);
			break;
		case 0x05: // DCR  B
		{
			cpu->b--;
			flagsZSP(cpu, cpu->b);

			break;
		}
		case 0x06: // MVI  B,d8
			cpu->b = opcode[1];
			cpu->pc++;

			break;
		case 0x07: // RLC
			unimplemented(opcode[0]);
			break;
		case 0x08: // 0x08 ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0x09: // DAD  B
		{
			uint16_t hl = (cpu->h << 8) | cpu->l;
			uint16_t add = (cpu->b << 8) | cpu->c;

			uint16_t res = hl + add;
			cpu->h = res >> 8;
			cpu->l = res & 0xff;
			cpu->flags.c = (res >> 16) & 1;

			break;
		}
		case 0x0a: // LDAX B
		{
			unimplemented(opcode[0]);
			uint16_t adr = (cpu->b << 8) | cpu->c;
			cpu->a = cpu->ram[adr];
			break;
		}
		case 0x0b: // DCX  B
			unimplemented(opcode[0]);
			break;
		case 0x0c: // INC  C
			unimplemented(opcode[0]);
			break;
		case 0x0d: // DCR  C
            cpu->c--;
            flagsZSP(cpu, cpu->c);

			break;
		case 0x0e: // MVI  C,d8
			cpu->c = opcode[1];
			cpu->pc++;

			break;
		case 0x0f: // RRC
		{
			uint8_t tmp = cpu->a;
			cpu->a = ((tmp & 1) << 7) | (tmp >> 1);
			cpu->flags.c = ((tmp & 1) == 1);

			break;
		}
		case 0x10: // 0x10 ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0x11: // LXI  D,d16
			cpu->e = opcode[1];
			cpu->d = opcode[2];

			cpu->pc += 2;

			break;
		case 0x12: // STAX D
			unimplemented(opcode[0]);
			unimplemented(opcode[0]);
			break;
		case 0x13: // INX  D
		{
            uint16_t de = (cpu->d << 8) | cpu->e;
            de++;
            cpu->d = de >> 8;
            cpu->e = de & 0xff;

			break;
		}
		case 0x14: // INR  D
			unimplemented(opcode[0]);
			break;
		case 0x15: // DCR  D
			unimplemented(opcode[0]);
			break;
		case 0x16: // MVI  D,d8
			unimplemented(opcode[0]);
			cpu->d = opcode[1];

			break;
		case 0x17: // RAL
			unimplemented(opcode[0]);
			break;
		case 0x18: // 0x18 ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0x19: // DAD  D
		{
			uint16_t hl = (cpu->h << 8) | cpu->l;
			uint16_t add = (cpu->d << 8) | cpu->e;

			uint16_t res = hl + add;
			cpu->h = res >> 8;
			cpu->l = res & 0xff;
			cpu->flags.c = (res >> 16) & 1;

			break;
		}
		case 0x1a: // LDAX D
		{
			uint16_t adr = (cpu->d << 8) | cpu->e;
			cpu->a = cpu->ram[adr];

			break;
		}
		case 0x1b: // DCX  D
			unimplemented(opcode[0]);
			break;
		case 0x1c: // INC  E
			unimplemented(opcode[0]);
			break;
		case 0x1d: // DCR  E
			unimplemented(opcode[0]);
			break;
		case 0x1e: // MVI  E,d8
			unimplemented(opcode[0]);
			cpu->e = opcode[1];

			break;
		case 0x1f: // RAR
			unimplemented(opcode[0]);
			break;
		case 0x20: // 0x20 ILLEGAL
			unimplemented(opcode[0]);
			break;

		case 0x21: // LXI  H,d16
			cpu->l = opcode[1];
			cpu->h = opcode[2];

            cpu->pc += 2;

			break;
		case 0x22: // SHLD a16
			unimplemented(opcode[0]);
			break;
		case 0x23: // INX  H
		{
			uint16_t hl = (cpu->h << 8) | cpu->l;
			hl++;
			cpu->h = hl >> 8;
			cpu->l = hl & 0xff;


			break;
		}
		case 0x24: // INR  H
			unimplemented(opcode[0]);
			break;
		case 0x25: // DCR  H
			unimplemented(opcode[0]);
			break;
		case 0x26: // MVI  H,d8
			cpu->h = opcode[1];
			cpu->pc++;

			break;
		case 0x27: // DAA
			unimplemented(opcode[0]);
			break;
		case 0x28: // 0x28 ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0x29: // DAD  H
		{
			uint16_t hl = (cpu->h << 8) | cpu->l;
			uint16_t add = (cpu->h << 8) | cpu->l;

			uint16_t res = hl + add;
			cpu->h = res >> 8;
			cpu->l = res & 0xff;

			cpu->flags.c = (res >> 16) & 1;

			break;
		}
		case 0x2a: // LHLD a16
			unimplemented(opcode[0]);
			break;
		case 0x2b: // DCX  H
			unimplemented(opcode[0]);
			break;
		case 0x2c: // INC  L
			unimplemented(opcode[0]);
			break;
		case 0x2d: // DCR  L
			unimplemented(opcode[0]);
			break;
		case 0x2e: // MVI  L,d8
			unimplemented(opcode[0]);
			cpu->l = opcode[1];

			break;
		case 0x2f: // CMA
			unimplemented(opcode[0]);
			cpu->a = ~cpu->a;
			break;
		case 0x30: // 0x30 ILLEGAL
			unimplemented(opcode[0]);
			break;

		case 0x31: // LXI  SP d16
			cpu->sp = (opcode[2] << 8) | opcode[1];
			cpu->pc += 2;

			break;
		case 0x32: // STA a16
		{
			uint16_t adr = (opcode[2] << 8) | opcode[1];
			cpu->ram[adr] = cpu->a;
			cpu->pc += 2;

			break;
		}
		case 0x33: // INX  SP
			unimplemented(opcode[0]);
			break;
		case 0x34: // INR  M
			unimplemented(opcode[0]);
			break;
		case 0x35: // DCR  M
		{
			unimplemented(opcode[0]);
			uint8_t ans = cpu->ram[(cpu->h << 8) | cpu->l];
			cpu->b = ans & 0xff;
			break;
		}
		case 0x36: // MVI  M,d8
		{
			uint16_t adr = (cpu->h << 8) | cpu->l;
			cpu->ram[adr] = opcode[1];
			cpu->pc++;

			break;
		}
		case 0x37: // STC
			unimplemented(opcode[0]);
			break;
		case 0x38: // 0x38 ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0x39: // DAD  SP
			unimplemented(opcode[0]);
			break;
		case 0x3a: // LDA a16
		{
			uint16_t adr = (opcode[2] << 8) | opcode[1];
			cpu->a = cpu->ram[adr];
			cpu->pc += 2;

			break;
		}
		case 0x3b: // DCX  SP
			unimplemented(opcode[0]);
			break;
		case 0x3c: // INC  A
			unimplemented(opcode[0]);
			break;
		case 0x3d: // DCR  A
			unimplemented(opcode[0]);
			break;
		case 0x3e: // MVI  A,d8
			cpu->a = opcode[1];
			cpu->pc++;

			break;
		case 0x3f: // CMC
			unimplemented(opcode[0]);
			break;

		case 0x40: // MOV B,B
			unimplemented(opcode[0]);
			cpu->b = cpu->b;
			break;
		case 0x41: // MOV B,C
			unimplemented(opcode[0]);
			cpu->b = cpu->c;
			break;
		case 0x42: // MOV B,D
			unimplemented(opcode[0]);
			cpu->b = cpu->d;
			break;
		case 0x43: // MOV B,E
			unimplemented(opcode[0]);
			cpu->b = cpu->e;
			break;
		case 0x44: // MOV B,H
			unimplemented(opcode[0]);
			cpu->b = cpu->h;
			break;
		case 0x45: // MOV B,L
			unimplemented(opcode[0]);
			cpu->b = cpu->l;
			break;
		case 0x46: // MOV B,M
			unimplemented(opcode[0]);
			break;
		case 0x47: // MOV B,A
			unimplemented(opcode[0]);
			cpu->b = cpu->a;
			break;

		case 0x48: // MOV C,B
			unimplemented(opcode[0]);
			cpu->c = cpu->b;
			break;
		case 0x49: // MOV C,C
			unimplemented(opcode[0]);
			cpu->c = cpu->c;
			break;
		case 0x4a: // MOV C,D
			unimplemented(opcode[0]);
			cpu->c = cpu->d;
			break;
		case 0x4b: // MOV C,E
			unimplemented(opcode[0]);
			cpu->c = cpu->e;
			break;
		case 0x4c: // MOV C,H
			unimplemented(opcode[0]);
			cpu->c = cpu->h;
			break;
		case 0x4d: // MOV C,L
			unimplemented(opcode[0]);
			cpu->c = cpu->l;
			break;
		case 0x4e: // MOV C,M
			unimplemented(opcode[0]);
			break;
		case 0x4f: // MOV C,A
			unimplemented(opcode[0]);
			cpu->c = cpu->a;
			break;

		case 0x50: // MOV D,B
			unimplemented(opcode[0]);
			cpu->d = cpu->b;
			break;
		case 0x51: // MOV D,C
			unimplemented(opcode[0]);
			cpu->d = cpu->c;
			break;
		case 0x52: // MOV D,D
			unimplemented(opcode[0]);
			cpu->d = cpu->d;
			break;
		case 0x53: // MOV D,E
			unimplemented(opcode[0]);
			cpu->d = cpu->e;
			break;
		case 0x54: // MOV D,H
			unimplemented(opcode[0]);
			cpu->d = cpu->h;
			break;
		case 0x55: // MOV D,L
			unimplemented(opcode[0]);
			cpu->d = cpu->l;
			break;
		case 0x56: // MOV D,M
		{
			uint8_t adr = (cpu->h << 8) | cpu->l;
			cpu->d = cpu->ram[adr];

			break;
		}
		case 0x57: // MOV D,A
			unimplemented(opcode[0]);
			cpu->d = cpu->a;
			break;

		case 0x58: // MOV E,B
			unimplemented(opcode[0]);
			cpu->e = cpu->b;
			break;
		case 0x59: // MOV E,C
			unimplemented(opcode[0]);
			cpu->e = cpu->c;
			break;
		case 0x5a: // MOV E,D
			unimplemented(opcode[0]);
			cpu->e = cpu->d;
			break;
		case 0x5b: // MOV E,E
			unimplemented(opcode[0]);
			cpu->e = cpu->e;
			break;
		case 0x5c: // MOV E,H
			unimplemented(opcode[0]);
			cpu->e = cpu->h;
			break;
		case 0x5d: // MOV E,L
			unimplemented(opcode[0]);
			cpu->e = cpu->l;
			break;
		case 0x5e: // MOV E,M
		{
			uint16_t adr = (cpu->h << 8) | cpu->l;
			cpu->e = cpu->ram[adr];

			break;
		}
		case 0x5f: // MOV E,A
			unimplemented(opcode[0]);
			cpu->e = cpu->a;
			break;

		case 0x60: // MOV H,B
			unimplemented(opcode[0]);
			cpu->h = cpu->b;
			break;
		case 0x61: // MOV H,C
			unimplemented(opcode[0]);
			cpu->h = cpu->c;
			break;
		case 0x62: // MOV H,D
			unimplemented(opcode[0]);
			cpu->h = cpu->d;
			break;
		case 0x63: // MOV H,E
			unimplemented(opcode[0]);
			cpu->h = cpu->e;
			break;
		case 0x64: // MOV H,H
			unimplemented(opcode[0]);
			cpu->h = cpu->h;
			break;
		case 0x65: // MOV H,L
			unimplemented(opcode[0]);
			cpu->h = cpu->l;
			break;
		case 0x66: // MOV H,M
		{
			uint16_t adr = (cpu->h << 8) | cpu->l;
			cpu->h = cpu->ram[adr];

			break;
		}
		case 0x67: // MOV H,A
			unimplemented(opcode[0]);
			cpu->h = cpu->a;
			break;

		case 0x68: // MOV L,B
			unimplemented(opcode[0]);
			cpu->l = cpu->b;
			break;
		case 0x69: // MOV L,C
			unimplemented(opcode[0]);
			cpu->l = cpu->c;
			break;
		case 0x6a: // MOV L,D
			unimplemented(opcode[0]);
			cpu->l = cpu->d;
			break;
		case 0x6b: // MOV L,E
			unimplemented(opcode[0]);
			cpu->l = cpu->e;
			break;
		case 0x6c: // MOV L,H
			unimplemented(opcode[0]);
			cpu->l = cpu->h;
			break;
		case 0x6d: // MOV L,L
			unimplemented(opcode[0]);
			cpu->l = cpu->l;
			break;
		case 0x6e: // MOV L,M
			unimplemented(opcode[0]);
			break;
		case 0x6f: // MOV L,A
			cpu->l = cpu->a;

			break;
		case 0x70: // MOV M,B
			unimplemented(opcode[0]);
			break;
		case 0x71: // MOV M,C
			unimplemented(opcode[0]);
			break;
		case 0x72: // MOV M,D
			unimplemented(opcode[0]);
			break;
		case 0x73: // MOV M,E
			unimplemented(opcode[0]);
			break;
		case 0x74: // MOV M,H
			unimplemented(opcode[0]);
			break;
		case 0x75: // MOV M,L
			unimplemented(opcode[0]);
			break;

		case 0x76: // HLT
			unimplemented(opcode[0]);
			return 1;
			break;
		case 0x77: // MOV M,A
		{
			uint16_t adr = (cpu->h << 8) | cpu->l;
			cpu->ram[adr] = cpu->a;

			break;
		}
		case 0x78: // MOV A,B
			unimplemented(opcode[0]);
			cpu->a = cpu->b;
			break;
		case 0x79: // MOV A,C
			unimplemented(opcode[0]);
			cpu->a = cpu->c;
			break;
		case 0x7a: // MOV A,D
			cpu->a = cpu->d;

			break;
		case 0x7b: // MOV A,E
			cpu->a = cpu->e;

			break;
		case 0x7c: // MOV A,H
			cpu->a = cpu->h;

			break;
		case 0x7d: // MOV A,L
			unimplemented(opcode[0]);
			cpu->a = cpu->l;
			break;
		case 0x7e: // MOV A,M
		{
			uint16_t adr = (cpu->h << 8) | cpu->l;
			cpu->a = cpu->ram[adr];

			break;
		}
		case 0x7f: // MOV A,A
			unimplemented(opcode[0]);
			cpu->a = cpu->h;
			break;

		case 0x80: // ADD B
			unimplemented(opcode[0]);
			break;
		case 0x81: // ADD C
			unimplemented(opcode[0]);

			break;
		case 0x82: // ADD D
			unimplemented(opcode[0]);

			break;
		case 0x83: // ADD E
			unimplemented(opcode[0]);

			break;
		case 0x84: // ADD H
			unimplemented(opcode[0]);

			break;
		case 0x85: // ADD L
			unimplemented(opcode[0]);

			break;
		case 0x86: // ADD M
		{
			unimplemented(opcode[0]);
			uint16_t adr = (cpu->h << 8) | cpu->l;

			break;
		}
		case 0x87: // ADD A
			unimplemented(opcode[0]);

			break;

		case 0x88: // ADC B
			unimplemented(opcode[0]);
			break;
		case 0x89: // ADC C
			unimplemented(opcode[0]);
			break;
		case 0x8a: // ADC D
			unimplemented(opcode[0]);
			break;
		case 0x8b: // ADC E
			unimplemented(opcode[0]);
			break;
		case 0x8c: // ADC H
			unimplemented(opcode[0]);
			break;
		case 0x8d: // ADC L
			unimplemented(opcode[0]);
			break;
		case 0x8e: // ADC M
			unimplemented(opcode[0]);
			break;
		case 0x8f: // ADC A
			unimplemented(opcode[0]);
			break;

		case 0x90: // SUB B
			unimplemented(opcode[0]);
			break;
		case 0x91: // SUB C
			unimplemented(opcode[0]);
			break;
		case 0x92: // SUB D
			unimplemented(opcode[0]);
			break;
		case 0x93: // SUB E
			unimplemented(opcode[0]);
			break;
		case 0x94: // SUB H
			unimplemented(opcode[0]);
			break;
		case 0x95: // SUB L
			unimplemented(opcode[0]);
			break;
		case 0x96: // SUB M
			unimplemented(opcode[0]);
			break;
		case 0x97: // SUB A
			unimplemented(opcode[0]);
			break;

		case 0x98: // SBB B
			unimplemented(opcode[0]);
			break;
		case 0x99: // SBB C
			unimplemented(opcode[0]);
			break;
		case 0x9a: // SBB D
			unimplemented(opcode[0]);
			break;
		case 0x9b: // SBB E
			unimplemented(opcode[0]);
			break;
		case 0x9c: // SBB H
			unimplemented(opcode[0]);
			break;
		case 0x9d: // SBB L
			unimplemented(opcode[0]);
			break;
		case 0x9e: // SBB M
			unimplemented(opcode[0]);
			break;
		case 0x9f: // SBB A
			unimplemented(opcode[0]);
			break;

		case 0xa0: // ANA B
			unimplemented(opcode[0]);
			cpu->a &= cpu->b;
			cpu->flags.c = 0;
			break;
		case 0xa1: // ANA C
			unimplemented(opcode[0]);
			break;
		case 0xa2: // ANA D
			unimplemented(opcode[0]);
			break;
		case 0xa3: // ANA E
			unimplemented(opcode[0]);
			break;
		case 0xa4: // ANA H
			unimplemented(opcode[0]);
			break;
		case 0xa5: // ANA L
			unimplemented(opcode[0]);
			break;
		case 0xa6: // ANA M
			unimplemented(opcode[0]);
			break;
		case 0xa7: // ANA A
			cpu->a &= cpu->a;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;

			break;

		case 0xa8: // XRA B
			unimplemented(opcode[0]);
			break;
		case 0xa9: // XRA C
			unimplemented(opcode[0]);
			break;
		case 0xaa: // XRA D
			unimplemented(opcode[0]);
			break;
		case 0xab: // XRA E
			unimplemented(opcode[0]);
			break;
		case 0xac: // XRA H
			unimplemented(opcode[0]);
			break;
		case 0xad: // XRA L
			unimplemented(opcode[0]);
			break;
		case 0xae: // XRA M
			unimplemented(opcode[0]);
			break;
		case 0xaf: // XRA A
			cpu->a ^= cpu->a;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;

			break;

		case 0xb0: // ORA B
			unimplemented(opcode[0]);
			break;
		case 0xb1: // ORA C
			unimplemented(opcode[0]);
			break;
		case 0xb2: // ORA D
			unimplemented(opcode[0]);
			break;
		case 0xb3: // ORA E
			unimplemented(opcode[0]);
			break;
		case 0xb4: // ORA H
			unimplemented(opcode[0]);
			break;
		case 0xb5: // ORA L
			unimplemented(opcode[0]);
			break;
		case 0xb6: // ORA M
			unimplemented(opcode[0]);
			break;
		case 0xb7: // ORA A
			unimplemented(opcode[0]);
			break;

		case 0xb8: // CMP B
			unimplemented(opcode[0]);
			break;
		case 0xb9: // CMP C
			unimplemented(opcode[0]);
			break;
		case 0xba: // CMP D
			unimplemented(opcode[0]);
			break;
		case 0xbb: // CMP E
			unimplemented(opcode[0]);
			break;
		case 0xbc: // CMP H
			unimplemented(opcode[0]);
			break;
		case 0xbd: // CMP L
			unimplemented(opcode[0]);
			break;
		case 0xbe: // CMP M
			unimplemented(opcode[0]);
			break;
		case 0xbf: // CMP A
			unimplemented(opcode[0]);
			break;

		case 0xc0: // RNZ
			unimplemented(opcode[0]);
			break;
		case 0xc1: // POP B
		{
			uint16_t val = pop(cpu);
			cpu->b = val >> 8;
			cpu->c = val & 0xff;

			break;
		}
		case 0xc2: // JNZ a16
			if (cpu->flags.z == 0) {
				cpu->pc = (opcode[2] << 8) | opcode[1];

			} else {
				cpu->pc += 2;

			}

			break;
		case 0xc3: // JMP a16
			cpu->pc = (opcode[2] << 8) | opcode[1];

			break;
		case 0xc4: // CNZ a16
			unimplemented(opcode[0]);
			break;
		case 0xc5: // PUSH B
			push(cpu, cpu->b, cpu->c);

			break;
		case 0xc6: // ADI d8
		{
			uint16_t tmp = cpu->a + opcode[1];
			flagsZSP(cpu, tmp);
			cpu->flags.c = tmp > 0xff;
			cpu->a += opcode[1];
			cpu->pc++;

			break;
		}
		case 0xc7: // RST 0
		{
			unimplemented(opcode[0]);
			uint16_t adr = cpu->pc + 3;
			cpu->ram[cpu->sp-1] = (adr >> 8) & 0xff;
			cpu->ram[cpu->sp-2] = adr & 0xff;
			cpu->sp -= 2;
			cpu->pc = 0x0000;

			break;
		}
		case 0xc8: // RZ
			unimplemented(opcode[0]);
			if (!cpu->flags.z) {

				break;
			}
			cpu->pc = cpu->ram[cpu->sp] | (cpu->ram[cpu->sp + 1] << 8);

			break;
		case 0xc9: // RET
		{
			uint16_t adr = pop(cpu);
			cpu->pc = adr;

			break;
		}
		case 0xca: // JZ a16
			unimplemented(opcode[0]);
			if (cpu->flags.z != 1) {

				break;
			}

			cpu->pc = (opcode[2] << 8) | opcode[1];

			break;
		case 0xcb: // 0xcb ILLEGAL
			unimplemented(opcode[0]);
			break;

		case 0xcc: // CZ a16
			unimplemented(opcode[0]);
			if (!cpu->flags.z) {

				break;
			}
			uint16_t adr = cpu->pc + 3;
			cpu->ram[cpu->sp-1] = (adr >> 8) & 0xff;
			cpu->ram[cpu->sp-2] = adr & 0xff;
			cpu->sp -= 2;
			cpu->pc = (opcode[2] << 8) | opcode[1];

			break;
		case 0xcd: // CALL a16
		{
			call(cpu, opcode);

			/*unimplemented(opcode[0]);*/
			/*uint16_t adr = cpu->pc + 3;*/
			/*cpu->ram[cpu->sp-1] = (adr >> 8) & 0xff;*/
			/*cpu->ram[cpu->sp-2] = adr & 0xff;*/
			/*cpu->sp -= 2;*/
			/*cpu->pc = (opcode[2] << 8) | opcode[1];*/

			break;
		}
		case 0xce: // ACI d8
			unimplemented(opcode[0]);
			break;
		case 0xcf: // RST 1
		{
			unimplemented(opcode[0]);
			uint16_t adr = cpu->pc + 3;
			cpu->ram[cpu->sp-1] = (adr >> 8) & 0xff;
			cpu->ram[cpu->sp-2] = adr & 0xff;
			cpu->sp -= 2;
			cpu->pc = 0x0008;

			break;
		}
		case 0xd0: // RNC
			unimplemented(opcode[0]);
			break;
		case 0xd1: // POP D
		{
			uint16_t val = pop(cpu);
			cpu->d = val >> 8;
			cpu->e = val & 0xff;


			break;
		}
		case 0xd2: // JNC a16
			unimplemented(opcode[0]);
			if (cpu->flags.c) {

				break;
			}

			cpu->pc = (opcode[2] << 8) | opcode[1];

			break;
		// TODO: Reimplement later
		case 0xd3: // OUT d8
			out(cpu, opcode[1]);
			cpu->pc++;
			break;
		case 0xd4: // CNC a16
			unimplemented(opcode[0]);
			break;
		case 0xd5: // PUSH D
			push(cpu, cpu->d, cpu->e);

			break;
		case 0xd6: // SUI d8
			unimplemented(opcode[0]);
			break;
		case 0xd7: // RST 2
			unimplemented(opcode[0]);
		{
			uint16_t adr = cpu->pc + 3;
			cpu->ram[cpu->sp-1] = (adr >> 8) & 0xff;
			cpu->ram[cpu->sp-2] = adr & 0xff;
			cpu->sp -= 2;
			cpu->pc = 0x0010;

			break;
		}
		case 0xd8: // RC
			unimplemented(opcode[0]);
			break;
		case 0xd9: // 0xd9 ILLEGAL
			unimplemented(opcode[0]);
			break;

		case 0xda: // JC a16
			unimplemented(opcode[0]);
			if (!cpu->flags.c) {

				break;
			}

			cpu->pc = (opcode[2] << 8) | opcode[1];

			break;
		// TODO: Reimplement later
		case 0xdb: // IN d8
			cpu->a = in(cpu, opcode[1]);
			cpu->pc++;
			break;
		case 0xdc: // CC a16
			unimplemented(opcode[0]);
			break;
		case 0xdd: // 0xdd ILLEGAL
			unimplemented(opcode[0]);
			break;

		case 0xde: // SBI d8
			unimplemented(opcode[0]);
			break;
		case 0xdf: // RST 3
		{
			unimplemented(opcode[0]);
			uint16_t adr = cpu->pc + 3;
			cpu->ram[cpu->sp-1] = (adr >> 8) & 0xff;
			cpu->ram[cpu->sp-2] = adr & 0xff;
			cpu->sp -= 2;
			cpu->pc = 0x0018;

			break;
		}
		case 0xe0: // RPO
			unimplemented(opcode[0]);
			break;
		case 0xe1: // POP H
		{
			uint16_t val = pop(cpu);
			cpu->h = val >> 8;
			cpu->l = val & 0xff;

			break;
		}
		case 0xe2: // JPO a16
			unimplemented(opcode[0]);
			if (cpu->flags.p == 1) {

				break;
			}

			cpu->pc = (opcode[2] << 8) | opcode[1];

			break;
		case 0xe3: // XTHL
			unimplemented(opcode[0]);
			break;
		case 0xe4: // CPO a16
			unimplemented(opcode[0]);
			break;
		case 0xe5: // PUSH H
			push(cpu, cpu->h, cpu->l);

			break;
		case 0xe6: // ANI d8
			cpu->a &= opcode[1];

			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;

			cpu->pc++;

			break;
		case 0xe7: // RST 4
		{
			unimplemented(opcode[0]);
			uint16_t adr = cpu->pc + 3;
			cpu->ram[cpu->sp-1] = (adr >> 8) & 0xff;
			cpu->ram[cpu->sp-2] = adr & 0xff;
			cpu->sp -= 2;
			cpu->pc = 0x0020;

			break;
		}
		case 0xe8: // RPE
			unimplemented(opcode[0]);
			break;
		case 0xe9: // PCHL
			unimplemented(opcode[0]);
			break;
		case 0xea: // JPE a16
			unimplemented(opcode[0]);
			if (cpu->flags.p == 0) {

				break;
			}

			cpu->pc = (opcode[2] << 8) | opcode[1];

			break;
		case 0xeb: // XCHG
		{
			uint8_t d = cpu->d;
			uint8_t e = cpu->e;

			cpu->d = cpu->h;
			cpu->e = cpu->l;

			cpu->h = d;
			cpu->l = e;


			break;
		}
		case 0xec: // CPE a16
			unimplemented(opcode[0]);
			break;
		case 0xed: // 0xed ILLEGAL
			unimplemented(opcode[0]);
			break;

		case 0xee: // XRI d8
			unimplemented(opcode[0]);
			break;
		case 0xef: // RST 5
		{
			unimplemented(opcode[0]);
			uint16_t adr = cpu->pc + 3;
			cpu->ram[cpu->sp-1] = (adr >> 8) & 0xff;
			cpu->ram[cpu->sp-2] = adr & 0xff;
			cpu->sp -= 2;
			cpu->pc = 0x0028;

			break;
		}
		case 0xf0: // RP
			unimplemented(opcode[0]);
			break;
		case 0xf1: // POP PSW
		{
			uint16_t af = pop(cpu);
			cpu->a = af >> 8;
			set_psw(&cpu->flags, af & 0xff);


			break;
		}
		case 0xf2: // JP a16
			unimplemented(opcode[0]);
			if (cpu->flags.s == 1) {

				break;
			}
			cpu->pc = (opcode[2] << 8) | opcode[1];

			break;
		case 0xf3: // DI
			unimplemented(opcode[0]);
			break;
		case 0xf4: // CP a16
			unimplemented(opcode[0]);
			break;
		case 0xf5: // PUSH PSW
		{
			uint8_t psw = get_psw(&cpu->flags);
			push(cpu, cpu->a, psw);

			break;
		}
		case 0xf6: // ORI d8
			unimplemented(opcode[0]);
			break;
		case 0xf7: // RST 6
		{
			unimplemented(opcode[0]);
			uint16_t adr = cpu->pc + 3;
			cpu->ram[cpu->sp-1] = (adr >> 8) & 0xff;
			cpu->ram[cpu->sp-2] = adr & 0xff;
			cpu->sp -= 2;
			cpu->pc = 0x0030;

			break;
		}
		case 0xf8: // RM
			unimplemented(opcode[0]);
			break;
		case 0xf9: // SPHL
			printf("%02x\n", cpu->pc);
			unimplemented(opcode[0]);
			break;
		case 0xfa: // JM a16
			unimplemented(opcode[0]);
			if (cpu->flags.s == 0) {

				break;
			}
			cpu->pc = (opcode[2] << 8) | opcode[1];

			break;
		case 0xfb: // EI
			cpu->interrupts = 1;

			break;
		case 0xfc: // CM a16
			unimplemented(opcode[0]);
			break;
		case 0xfd: // 0xfd ILLEGAL
			unimplemented(opcode[0]);
			break;

		case 0xfe: // CPI d8
		{
			uint16_t res = cpu->a - opcode[1];
			flagsZSP(cpu, res & 0xff);
			cpu->flags.c = res >> 8;
			cpu->pc++;
			break;
		}
		case 0xff: // RST 7
		{
			unimplemented(opcode[0]);
			uint16_t adr = cpu->pc + 3;
			cpu->ram[cpu->sp-1] = (adr >> 8) & 0xff;
			cpu->ram[cpu->sp-2] = adr & 0xff;
			cpu->sp -= 2;
			cpu->pc = 0x0038;
			break;
		}
	}

	return cycles8080[*opcode];
}


void print_cpu_state(struct CPU *cpu, int cycles) {
	struct Flags *flags = &cpu->flags;

	uint8_t psw = get_psw(flags);
	printf("cycles: %04d ", cycles);
	printf("af: %02x%02x ", cpu->a, psw);
	printf("bc: %02x%02x ", cpu->b, cpu->c);
	printf("de: %02x%02x ", cpu->d, cpu->e);
	printf("hl: %02x%02x ", cpu->h, cpu->l);
	printf("pc: %04x ", cpu->pc);
	printf("sp: %04x ", cpu->sp);

	printf("%c%c%c%c%c ",
		cpu->flags.z ? 'z' : '-',
		cpu->flags.s ? 's' : '-',
		cpu->flags.p ? 'p' : '-',
		cpu->interrupts ? 'i' : '-',
		cpu->flags.c ? 'c' : '-'
		);
	printf("stack: %02x %02x\n", cpu->ram[cpu->sp], cpu->ram[cpu->sp + 1]);
}
