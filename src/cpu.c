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
	cpu->ram = calloc(0x3FFF, sizeof(uint8_t));

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
	cpu->ram[cpu->sp - 1] = high;
	cpu->ram[cpu->sp - 2] = low;
	cpu->sp -= 2;
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
flagsZSP(struct CPU *cpu, uint16_t num) {
	cpu->flags.z = (num == 0);
	cpu->flags.s = ((num & 0x80) == 0x80);
	cpu->flags.p = parity(num, 8);
}

static void
flagsZSPC(struct CPU *cpu, uint16_t num) {
	flagsZSP(cpu, num);
	cpu->flags.c = num > 0xff;
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

static void
ret(struct CPU *cpu) {
	uint16_t adr = pop(cpu);
	cpu->pc = adr;
}

void
generate_interrupt(struct CPU *cpu, int interrupt_num) {
	push(cpu, cpu->pc >> 8, cpu->pc & 0xff);
	cpu->pc = 8 * interrupt_num;
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
		{
			uint16_t adr = cpu->b << 8 | cpu->c;
			cpu->ram[adr] = cpu->a;
			break;
		}
		case 0x03: // INX  B
			cpu->c++;
			if (cpu->c == 0)
				cpu->b++;
			break;
		case 0x04: // INR  B
			cpu->b++;
			flagsZSP(cpu, cpu->b);
			break;
		case 0x05: // DCR  B
			cpu->b--;
			flagsZSP(cpu, cpu->b);
			break;
		case 0x06: // MVI  B,d8
			cpu->b = opcode[1];
			cpu->pc++;
			break;
		case 0x07: // RLC
		{
			uint8_t x = cpu->a;
			cpu->a = (x << 1) | ((x & (1 << 7)) >> 7);
			cpu->flags.c = x >> 7;
			break;
		}
		case 0x08: // 0x08 ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0x09: // DAD  B
		{
			uint16_t hl = (cpu->h << 8) | cpu->l;
			uint16_t add = (cpu->b << 8) | cpu->c;

			uint32_t res = hl + add;
			cpu->h = res >> 8;
			cpu->l = res & 0xff;
			cpu->flags.c = (res >> 16) & 1;
			break;
		}
		case 0x0a: // LDAX B
		{
			uint16_t adr = (cpu->b << 8) | cpu->c;
			cpu->a = cpu->ram[adr];
			break;
		}
		case 0x0b: // DCX  B
		{
			uint16_t bc = cpu->b << 8 | cpu->c;
			bc--;
			cpu->b = bc >> 8;
			cpu->c = bc & 0xff;
			break;
		}
		case 0x0c: // INR  C
            cpu->c++;
            flagsZSP(cpu, cpu->c);
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
			uint8_t x = cpu->a;
			cpu->a = ((x & 1) << 7) | (x >> 1);
			cpu->flags.c = ((x & 1) == 1);
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
		{
			uint16_t adr = cpu->d << 8 | cpu->e;
			cpu->ram[adr] = cpu->a;
			break;
		}
		case 0x13: // INX  D
		{
            uint16_t de = (cpu->d << 8) | cpu->e;
            de++;
            cpu->d = de >> 8;
            cpu->e = de & 0xff;
			break;
		}
		case 0x14: // INR  D
			cpu->d++;
			flagsZSP(cpu, cpu->d);
			break;
		case 0x15: // DCR  D
			cpu->d--;
			flagsZSP(cpu, cpu->d);
			break;
		case 0x16: // MVI  D,d8
			cpu->d = opcode[1];
			cpu->pc++;
			break;
		case 0x17: // RAL
		{
			uint8_t x = cpu->a;
			cpu->a <<= 1;
			cpu->a |= cpu->flags.c;
			cpu->flags.c = x >> 7;
			break;
		}
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
		{
			uint16_t de = cpu->d << 8 | cpu->e;
			de--;
			cpu->d = de >> 8;
			cpu->e = de & 0xff;
			break;
		}
		case 0x1c: // INR  E
			cpu->e++;
			flagsZSP(cpu, cpu->e);
			break;
		case 0x1d: // DCR  E
			cpu->e--;
			flagsZSP(cpu, cpu->e);
			break;
		case 0x1e: // MVI  E,d8
			cpu->e = opcode[1];
			cpu->pc++;
			break;
		case 0x1f: // RAR
		{
			uint8_t x = cpu->a;
			cpu->a = (cpu->flags.c << 7) | (x >> 1);
			cpu->flags.c = (1 == (x & 1));
			break;
		}
		case 0x20: // 0x20 ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0x21: // LXI  H,d16
			cpu->l = opcode[1];
			cpu->h = opcode[2];
            cpu->pc += 2;
			break;
		case 0x22: // SHLD a16
		{
			uint16_t adr = opcode[2] << 8 | opcode[1];
			cpu->ram[adr + 1] = cpu->h;
			cpu->ram[adr] = cpu->l;
			cpu->pc += 2;
			break;
		}
		case 0x23: // INX  H
		{
			uint16_t hl = (cpu->h << 8) | cpu->l;
			hl++;
			cpu->h = hl >> 8;
			cpu->l = hl & 0xff;
			break;
		}
		case 0x24: // INR  H
			cpu->h++;
			flagsZSP(cpu, cpu->h);
			break;
		case 0x25: // DCR  H
			cpu->h--;
			flagsZSP(cpu, cpu->h);
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
		{
			uint16_t adr = opcode[2] << 8 | opcode[1];
			cpu->h = cpu->ram[adr + 1];
			cpu->l = cpu->ram[adr];
			cpu->pc += 2;
			break;
		}
		case 0x2b: // DCX  H
		{
			uint16_t hl = cpu->h << 8 | cpu->l;
			hl--;
			cpu->h = hl >> 8;
			cpu->l = hl & 0xff;
		}
			break;
		case 0x2c: // INR  L
			cpu->l++;
			flagsZSP(cpu, cpu->l);
			break;
		case 0x2d: // DCR  L
			cpu->l--;
			flagsZSP(cpu, cpu->l);
			break;
		case 0x2e: // MVI  L,d8
			cpu->l = opcode[1];
			cpu->pc++;
			break;
		case 0x2f: // CMA
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
			cpu->sp++;
			break;
		case 0x34: // INR  M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->ram[adr]++;
			flagsZSP(cpu, cpu->ram[adr]);
			break;
		}
		case 0x35: // DCR  M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->ram[adr]--;
			flagsZSP(cpu, cpu->ram[adr]);
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
			cpu->flags.c = 1;
			break;
		case 0x38: // 0x38 ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0x39: // DAD  SP
		{
			uint16_t hl = (cpu->h << 8) | cpu->l;

			uint16_t res = hl + cpu->sp;
			cpu->h = res >> 8;
			cpu->l = res & 0xff;
			cpu->flags.c = (res >> 16) & 1;
			break;
		}
		case 0x3a: // LDA a16
		{
			uint16_t adr = (opcode[2] << 8) | opcode[1];
			cpu->a = cpu->ram[adr];
			cpu->pc += 2;
			break;
		}
		case 0x3b: // DCX  SP
			cpu->sp--;
			break;
		case 0x3c: // INR  A
			cpu->a++;
			flagsZSP(cpu, cpu->a);
			break;
		case 0x3d: // DCR  A
			cpu->a--;
			flagsZSP(cpu, cpu->a);
			break;
		case 0x3e: // MVI  A,d8
			cpu->a = opcode[1];
			cpu->pc++;
			break;
		case 0x3f: // CMC
			cpu->flags.c = ~cpu->flags.c;
			break;
		case 0x40: // MOV B,B
			cpu->b = cpu->b;
			break;
		case 0x41: // MOV B,C
			cpu->b = cpu->c;
			break;
		case 0x42: // MOV B,D
			cpu->b = cpu->d;
			break;
		case 0x43: // MOV B,E
			cpu->b = cpu->e;
			break;
		case 0x44: // MOV B,H
			cpu->b = cpu->h;
			break;
		case 0x45: // MOV B,L
			cpu->b = cpu->l;
			break;
		case 0x46: // MOV B,M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->b = cpu->ram[adr];
			break;
		}
		case 0x47: // MOV B,A
			cpu->b = cpu->a;
			break;
		case 0x48: // MOV C,B
			cpu->c = cpu->b;
			break;
		case 0x49: // MOV C,C
			cpu->c = cpu->c;
			break;
		case 0x4a: // MOV C,D
			cpu->c = cpu->d;
			break;
		case 0x4b: // MOV C,E
			cpu->c = cpu->e;
			break;
		case 0x4c: // MOV C,H
			cpu->c = cpu->h;
			break;
		case 0x4d: // MOV C,L
			cpu->c = cpu->l;
			break;
		case 0x4e: // MOV C,M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->c = cpu->ram[adr];
			break;
		}
		case 0x4f: // MOV C,A
			cpu->c = cpu->a;
			break;
		case 0x50: // MOV D,B
			cpu->d = cpu->b;
			break;
		case 0x51: // MOV D,C
			cpu->d = cpu->c;
			break;
		case 0x52: // MOV D,D
			cpu->d = cpu->d;
			break;
		case 0x53: // MOV D,E
			cpu->d = cpu->e;
			break;
		case 0x54: // MOV D,H
			cpu->d = cpu->h;
			break;
		case 0x55: // MOV D,L
			cpu->d = cpu->l;
			break;
		case 0x56: // MOV D,M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->d = cpu->ram[adr];
			break;
		}
		case 0x57: // MOV D,A
			cpu->d = cpu->a;
			break;
		case 0x58: // MOV E,B
			cpu->e = cpu->b;
			break;
		case 0x59: // MOV E,C
			cpu->e = cpu->c;
			break;
		case 0x5a: // MOV E,D
			cpu->e = cpu->d;
			break;
		case 0x5b: // MOV E,E
			cpu->e = cpu->e;
			break;
		case 0x5c: // MOV E,H
			cpu->e = cpu->h;
			break;
		case 0x5d: // MOV E,L
			cpu->e = cpu->l;
			break;
		case 0x5e: // MOV E,M
		{
			uint16_t adr = (cpu->h << 8) | cpu->l;
			cpu->e = cpu->ram[adr];
			break;
		}
		case 0x5f: // MOV E,A
			cpu->e = cpu->a;
			break;
		case 0x60: // MOV H,B
			cpu->h = cpu->b;
			break;
		case 0x61: // MOV H,C
			cpu->h = cpu->c;
			break;
		case 0x62: // MOV H,D
			cpu->h = cpu->d;
			break;
		case 0x63: // MOV H,E
			cpu->h = cpu->e;
			break;
		case 0x64: // MOV H,H
			cpu->h = cpu->h;
			break;
		case 0x65: // MOV H,L
			cpu->h = cpu->l;
			break;
		case 0x66: // MOV H,M
		{
			uint16_t adr = (cpu->h << 8) | cpu->l;
			cpu->h = cpu->ram[adr];
			break;
		}
		case 0x67: // MOV H,A
			cpu->h = cpu->a;
			break;
		case 0x68: // MOV L,B
			cpu->l = cpu->b;
			break;
		case 0x69: // MOV L,C
			cpu->l = cpu->c;
			break;
		case 0x6a: // MOV L,D
			cpu->l = cpu->d;
			break;
		case 0x6b: // MOV L,E
			cpu->l = cpu->e;
			break;
		case 0x6c: // MOV L,H
			cpu->l = cpu->h;
			break;
		case 0x6d: // MOV L,L
			cpu->l = cpu->l;
			break;
		case 0x6e: // MOV L,M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->l = cpu->ram[adr];
			break;
		}
		case 0x6f: // MOV L,A
			cpu->l = cpu->a;
			break;
		case 0x70: // MOV M,B
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->ram[adr] = cpu->b;
			break;
		}
		case 0x71: // MOV M,C
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->ram[adr] = cpu->c;
			break;
		}
		case 0x72: // MOV M,D
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->ram[adr] = cpu->d;
			break;
		}
		case 0x73: // MOV M,E
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->ram[adr] = cpu->e;
			break;
		}
		case 0x74: // MOV M,H
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->ram[adr] = cpu->h;
			break;
		}
		case 0x75: // MOV M,L
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->ram[adr] = cpu->l;
			break;
		}
		case 0x76: // HLT
			exit(1);
			break;
		case 0x77: // MOV M,A
		{
			uint16_t adr = (cpu->h << 8) | cpu->l;
			cpu->ram[adr] = cpu->a;
			break;
		}
		case 0x78: // MOV A,B
			cpu->a = cpu->b;
			break;
		case 0x79: // MOV A,C
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
			flagsZSPC(cpu, cpu->a + cpu->b);
			cpu->a += cpu->b;
			break;
		case 0x81: // ADD C
			flagsZSPC(cpu, cpu->a + cpu->c);
			cpu->a += cpu->c;
			break;
		case 0x82: // ADD D
			flagsZSPC(cpu, cpu->a + cpu->d);
			cpu->a += cpu->d;
			break;
		case 0x83: // ADD E
			flagsZSPC(cpu, cpu->a + cpu->e);
			cpu->a += cpu->e;
			break;
		case 0x84: // ADD H
			flagsZSPC(cpu, cpu->a + cpu->h);
			cpu->a += cpu->h;
			break;
		case 0x85: // ADD L
			flagsZSPC(cpu, cpu->a + cpu->l);
			cpu->a += cpu->l;
			break;
		case 0x86: // ADD M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			flagsZSPC(cpu, cpu->a + cpu->ram[adr]);
			cpu->a += cpu->ram[adr];
			break;
		}
		case 0x87: // ADD A
			flagsZSPC(cpu, cpu->a + cpu->a);
			cpu->a += cpu->a;
			break;
		case 0x88: // ADC B
			cpu->a += cpu->b + cpu->flags.c;
			flagsZSPC(cpu, cpu->a + cpu->b + cpu->flags.c);
			break;
		case 0x89: // ADC C
			cpu->a += cpu->c + cpu->flags.c;
			flagsZSPC(cpu, cpu->a + cpu->c + cpu->flags.c);
			break;
		case 0x8a: // ADC D
			cpu->a += cpu->d + cpu->flags.c;
			flagsZSPC(cpu, cpu->a + cpu->d + cpu->flags.c);
			break;
		case 0x8b: // ADC E
			cpu->a += cpu->e + cpu->flags.c;
			flagsZSPC(cpu, cpu->a + cpu->e + cpu->flags.c);
			break;
		case 0x8c: // ADC H
			cpu->a += cpu->h + cpu->flags.c;
			flagsZSPC(cpu, cpu->a + cpu->h + cpu->flags.c);
			break;
		case 0x8d: // ADC L
			cpu->a += cpu->l + cpu->flags.c;
			flagsZSPC(cpu, cpu->a + cpu->l + cpu->flags.c);
			break;
		case 0x8e: // ADC M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->a += cpu->ram[adr] + cpu->flags.c;
			flagsZSPC(cpu, cpu->a + cpu->ram[adr] + cpu->flags.c);
			break;
		}
		case 0x8f: // ADC A
			cpu->a += cpu->a + cpu->flags.c;
			flagsZSPC(cpu, cpu->a + cpu->a + cpu->flags.c);
			break;
		case 0x90: // SUB B
			flagsZSPC(cpu, cpu->a - cpu->b);
			cpu->a -= cpu->b;
			break;
		case 0x91: // SUB C
			flagsZSPC(cpu, cpu->a - cpu->c);
			cpu->a -= cpu->c;
			break;
		case 0x92: // SUB D
			flagsZSPC(cpu, cpu->a - cpu->d);
			cpu->a -= cpu->d;
			break;
		case 0x93: // SUB E
			flagsZSPC(cpu, cpu->a - cpu->e);
			cpu->a -= cpu->e;
			break;
		case 0x94: // SUB H
			flagsZSPC(cpu, cpu->a - cpu->h);
			cpu->a -= cpu->h;
			break;
		case 0x95: // SUB L
			flagsZSPC(cpu, cpu->a - cpu->l);
			cpu->a -= cpu->l;
			break;
		case 0x96: // SUB M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			flagsZSPC(cpu, cpu->a - cpu->ram[adr]);
			cpu->a -= cpu->ram[adr];
			break;
		}
		case 0x97: // SUB A
			flagsZSPC(cpu, cpu->a - cpu->a);
			cpu->a -= cpu->e;
			break;
		case 0x98: // SBB B
			cpu->a += - cpu->b - cpu->flags.c;
			flagsZSPC(cpu, cpu->a - cpu->a - cpu->flags.c);
			break;
		case 0x99: // SBB C
			cpu->a += - cpu->c - cpu->flags.c;
			flagsZSPC(cpu, cpu->a - cpu->c - cpu->flags.c);
			break;
		case 0x9a: // SBB D
			cpu->a += - cpu->d - cpu->flags.c;
			flagsZSPC(cpu, cpu->a - cpu->d - cpu->flags.c);
			break;
		case 0x9b: // SBB E
			cpu->a += - cpu->e - cpu->flags.c;
			flagsZSPC(cpu, cpu->a - cpu->e - cpu->flags.c);
			break;
		case 0x9c: // SBB H
			cpu->a += - cpu->h - cpu->flags.c;
			flagsZSPC(cpu, cpu->a - cpu->h - cpu->flags.c);
			break;
		case 0x9d: // SBB L
			cpu->a += - cpu->l - cpu->flags.c;
			flagsZSPC(cpu, cpu->a - cpu->l - cpu->flags.c);
			break;
		case 0x9e: // SBB M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->a += - cpu->ram[adr] - cpu->flags.c;
			flagsZSPC(cpu, cpu->a - cpu->ram[adr] - cpu->flags.c);
			break;
		}
		case 0x9f: // SBB A
			cpu->a += - cpu->a - cpu->flags.c;
			flagsZSPC(cpu, cpu->a - cpu->a - cpu->flags.c);
			break;
		case 0xa0: // ANA B
			cpu->a &= cpu->b;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xa1: // ANA C
			cpu->a &= cpu->c;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xa2: // ANA D
			cpu->a &= cpu->d;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xa3: // ANA E
			cpu->a &= cpu->e;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xa4: // ANA H
			cpu->a &= cpu->h;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xa5: // ANA L
			cpu->a &= cpu->l;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xa6: // ANA M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->a &= cpu->ram[adr];
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		}
		case 0xa7: // ANA A
			cpu->a &= cpu->a;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xa8: // XRA B
			cpu->a ^= cpu->b;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xa9: // XRA C
			cpu->a ^= cpu->c;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xaa: // XRA D
			cpu->a ^= cpu->d;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xab: // XRA E
			cpu->a ^= cpu->e;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xac: // XRA H
			cpu->a ^= cpu->h;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xad: // XRA L
			cpu->a ^= cpu->l;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xae: // XRA M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->a ^= cpu->ram[adr];
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		}
		case 0xaf: // XRA A
			cpu->a ^= cpu->a;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xb0: // ORA B
			cpu->a |= cpu->b;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xb1: // ORA C
			cpu->a |= cpu->c;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xb2: // ORA D
			cpu->a |= cpu->d;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xb3: // ORA E
			cpu->a |= cpu->e;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xb4: // ORA H
			cpu->a |= cpu->h;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xb5: // ORA L
			cpu->a |= cpu->l;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xb6: // ORA M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			cpu->a |= cpu->ram[adr];
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		}
		case 0xb7: // ORA A
			cpu->a |= cpu->a;
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			break;
		case 0xb8: // CMP B
			flagsZSPC(cpu, cpu->a - cpu->b);
			break;
		case 0xb9: // CMP C
			flagsZSPC(cpu, cpu->a - cpu->c);
			break;
		case 0xba: // CMP D
			flagsZSPC(cpu, cpu->a - cpu->d);
			break;
		case 0xbb: // CMP E
			flagsZSPC(cpu, cpu->a - cpu->e);
			break;
		case 0xbc: // CMP H
			flagsZSPC(cpu, cpu->a - cpu->h);
			break;
		case 0xbd: // CMP L
			flagsZSPC(cpu, cpu->a - cpu->l);
			break;
		case 0xbe: // CMP M
		{
			uint16_t adr = cpu->h << 8 | cpu->l;
			flagsZSPC(cpu, cpu->a - cpu->ram[adr]);
			break;
		}
		case 0xbf: // CMP A
			flagsZSPC(cpu, cpu->a - cpu->a);
			break;
		case 0xc0: // RNZ
			if (cpu->flags.z == 0)
				ret(cpu);
			break;
		case 0xc1: // POP B
		{
			uint16_t val = pop(cpu);
			cpu->b = val >> 8;
			cpu->c = val & 0xff;

			break;
		}
		case 0xc2: // JNZ a16
			if (cpu->flags.z == 0)
				cpu->pc = (opcode[2] << 8) | opcode[1];
			else
				cpu->pc += 2;
			break;
		case 0xc3: // JMP a16
			cpu->pc = (opcode[2] << 8) | opcode[1];
			break;
		case 0xc4: // CNZ a16
			if (cpu->flags.z == 0)
				call(cpu, opcode);
			else
				cpu->pc += 2;
			break;
		case 0xc5: // PUSH B
			push(cpu, cpu->b, cpu->c);
			break;
		case 0xc6: // ADI d8
		{
			uint16_t tmp = cpu->a + opcode[1];
			flagsZSP(cpu, tmp & 0xff);
			cpu->flags.c = tmp > 0xff;
			cpu->a += opcode[1];
			cpu->pc++;
			break;
		}
		case 0xc7: // RST 0
			unimplemented(opcode[0]);
			break;
		case 0xc8: // RZ
			if (cpu->flags.z)
				ret(cpu);
			break;
		case 0xc9: // RET
			ret(cpu);
			break;
		case 0xca: // JZ a16
			if (cpu->flags.z)
				cpu->pc = (opcode[2] << 8) | opcode[1];
			else
				cpu->pc += 2;
			break;
		case 0xcb: // 0xcb ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0xcc: // CZ a16
			if (cpu->flags.z)
				call(cpu, opcode);
			else
				cpu->pc += 2;
			break;
		case 0xcd: // CALL a16
		{
			call(cpu, opcode);
			break;
		}
		case 0xce: // ACI d8
		{
			uint16_t tmp = cpu->a + opcode[1] + cpu->flags.c;
			flagsZSP(cpu, tmp & 0xff);
			cpu->flags.c = tmp > 0xff;
			cpu->a = tmp & 0xff;
			cpu->pc++;
			break;
		}
		case 0xcf: // RST 1
			unimplemented(opcode[0]);
			break;
		case 0xd0: // RNC
			if (cpu->flags.c == 0)
				ret(cpu);
			break;
		case 0xd1: // POP D
		{
			uint16_t val = pop(cpu);
			cpu->d = val >> 8;
			cpu->e = val & 0xff;
			break;
		}
		case 0xd2: // JNC a16
			if (cpu->flags.c != 1)
				cpu->pc = (opcode[2] << 8) | opcode[1];
			else
				cpu->pc += 2;
			break;
		case 0xd3: // OUT d8
			out(cpu, opcode[1]);
			cpu->pc++;
			break;
		case 0xd4: // CNC a16
			if (cpu->flags.c == 0)
				call(cpu, opcode);
			else
				cpu->pc += 2;
			break;
		case 0xd5: // PUSH D
			push(cpu, cpu->d, cpu->e);
			break;
		case 0xd6: // SUI d8
		{
			uint8_t tmp = cpu->a - opcode[1];
			flagsZSP(cpu, tmp);
			cpu->flags.c = cpu->a < opcode[1];
			cpu->a = tmp;
			cpu->pc++;
			break;
		}
		case 0xd7: // RST 2
			unimplemented(opcode[0]);
			break;
		case 0xd8: // RC
			if (cpu->flags.c)
				ret(cpu);
			break;
		case 0xd9: // 0xd9 ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0xda: // JC a16
			if (cpu->flags.c)
				cpu->pc = (opcode[2] << 8) | opcode[1];
			else
				cpu->pc += 2;
			break;
		case 0xdb: // IN d8
			cpu->a = in(cpu, opcode[1]);
			cpu->pc++;
			break;
		case 0xdc: // CC a16
			if (cpu->flags.c)
				call(cpu, opcode);
			else
				cpu->pc += 2;
			break;
		case 0xdd: // 0xdd ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0xde: // SBI d8
		{
			uint16_t a = cpu->a + (cpu->flags.c << 8);
			uint16_t tmp = a - opcode[1];
			flagsZSP(cpu, tmp & 0xff);
			cpu->flags.c = a >> 8;
			cpu->pc++;
			break;
		}
		case 0xdf: // RST 3
			unimplemented(opcode[0]);
			break;
		case 0xe0: // RPO
			if (cpu->flags.p == 0)
				ret(cpu);
			break;
		case 0xe1: // POP H
		{
			uint16_t val = pop(cpu);
			cpu->h = val >> 8;
			cpu->l = val & 0xff;
			break;
		}
		case 0xe2: // JPO a16
			if (cpu->flags.p == 0)
				cpu->pc = (opcode[2] << 8) | opcode[1];
			else
				cpu->pc += 2;
			break;
		case 0xe3: // XTHL
		{
			uint16_t val = pop(cpu);
			uint16_t hl = cpu->h << 8 | cpu->l;
			cpu->h = val >> 8;
			cpu->l = val & 0xff;
			push(cpu, hl >> 8, hl & 0xff);
			break;
		}
		case 0xe4: // CPO a16
			if (cpu->flags.p == 0)
				call(cpu, opcode);
			else
				cpu->pc += 2;
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
			unimplemented(opcode[0]);
			break;
		case 0xe8: // RPE
			if (cpu->flags.p)
				ret(cpu);
			break;
		case 0xe9: // PCHL
			cpu->pc = cpu->h << 8 | cpu->l;
			break;
		case 0xea: // JPE a16
			if (cpu->flags.p == 1)
				cpu->pc = (opcode[2] << 8) | opcode[1];
			else
				cpu->pc += 2;
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
			if (cpu->flags.p)
				call(cpu, opcode);
			else
				cpu->pc += 2;
			break;
		case 0xed: // 0xed ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0xee: // XRI d8
			cpu->a ^= opcode[1];
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			cpu->pc++;
			break;
		case 0xef: // RST 5
			unimplemented(opcode[0]);
			break;
		case 0xf0: // RP
			if (cpu->flags.s == 0)
				ret(cpu);
			break;
		case 0xf1: // POP PSW
		{
			uint16_t af = pop(cpu);
			cpu->a = af >> 8;
			set_psw(&cpu->flags, af & 0xff);
			break;
		}
		case 0xf2: // JP a16
			if (cpu->flags.s == 0)
				cpu->pc = (opcode[2] << 8) | opcode[1];
			else
				cpu->pc += 2;
			break;
		case 0xf3: // DI
			cpu->interrupts = 0;
			break;
		case 0xf4: // CP a16
			if (cpu->flags.s == 0)
				call(cpu, opcode);
			else
				cpu->pc += 2;
			break;
		case 0xf5: // PUSH PSW
		{
			uint8_t psw = get_psw(&cpu->flags);
			push(cpu, cpu->a, psw);
			break;
		}
		case 0xf6: // ORI d8
			cpu->a |= opcode[1];
			flagsZSP(cpu, cpu->a);
			cpu->flags.c = 0;
			cpu->pc++;
			break;
		case 0xf7: // RST 6
			unimplemented(opcode[0]);
			break;
		case 0xf8: // RM
			if (cpu->flags.s)
				ret(cpu);
			break;
		case 0xf9: // SPHL
		{
			uint16_t hl = cpu->h << 8 | cpu->l;
			cpu->sp = hl;
			break;
		}
		case 0xfa: // JM a16
			if (cpu->flags.s == 1)
				cpu->pc = (opcode[2] << 8) | opcode[1];
			else
				cpu->pc += 2;
			break;
		case 0xfb: // EI
			cpu->interrupts = 1;
			break;
		case 0xfc: // CM a16
			if (cpu->flags.s == 1)
				call(cpu, opcode);
			else
				cpu->pc += 2;
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
			unimplemented(opcode[0]);
			break;
	}

	return cycles8080[*opcode];
}


void print_cpu_state(struct CPU *cpu, int cycles) {
	struct Flags *flags = &cpu->flags;

	uint8_t psw = get_psw(flags);
	printf("->%02x ", cpu->ram[cpu->pc]);
	printf("cycles: %04d ", cycles);
	printf("af: %02x%02x ", cpu->a, psw);
	printf("bc: %02x%02x ", cpu->b, cpu->c);
	printf("de: %02x%02x ", cpu->d, cpu->e);
	printf("hl: %02x%02x ", cpu->h, cpu->l);
	printf("pc: %04x ", cpu->pc);
	printf("sp: %04x ", cpu->sp);
	printf("m: %02x ", cpu->ram[cpu->h << 8 | cpu->l]);

	printf("%c%c%c%c%c ",
		cpu->flags.z ? 'z' : '-',
		cpu->flags.s ? 's' : '-',
		cpu->flags.p ? 'p' : '-',
		cpu->interrupts ? 'i' : '-',
		cpu->flags.c ? 'c' : '-'
		);
	printf("stack: %02x %02x\n", cpu->ram[cpu->sp], cpu->ram[cpu->sp + 1]);
}
