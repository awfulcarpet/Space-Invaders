#include <stdint.h>
#include <stdlib.h>

#include "cpu.h"

void unimplemented(uint8_t opcode) {
	fprintf(stderr, "unimplemented: %02x\n", opcode);
	exit(1);
}

int
emulate(struct CPU *cpu) {
	uint8_t *opcode = &cpu->ram[cpu->registers.pc];
	int bytes = 1;
	switch (*opcode) {
		case 0x00: // NOP
			unimplemented(opcode[0]);
			break;
		case 0x01: // LXI  B,d16
			unimplemented(opcode[0]);
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
			unimplemented(opcode[0]);
			break;
		case 0x06: // MVI  B,d8
			unimplemented(opcode[0]);
			break;
		case 0x07: // RLC
			unimplemented(opcode[0]);
			break;
		case 0x08: // 0x08 ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0x09: // DAD  B
			unimplemented(opcode[0]);
			break;
		case 0x0a: // LDAX B
			unimplemented(opcode[0]);
			break;
		case 0x0b: // DCX  B
			unimplemented(opcode[0]);
			break;
		case 0x0c: // INC  C
			unimplemented(opcode[0]);
			break;
		case 0x0d: // DCR  C
			unimplemented(opcode[0]);
			break;
		case 0x0e: // MVI  C,d8
			unimplemented(opcode[0]);
			break;
		case 0x0f: // RRC
			unimplemented(opcode[0]);
			break;
		case 0x10: // 0x10 ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0x11: // LXI  D,d16
			unimplemented(opcode[0]);
			break;
		case 0x12: // STAX D
			unimplemented(opcode[0]);
			break;
		case 0x13: // INX  D
			unimplemented(opcode[0]);
			break;
		case 0x14: // INR  D
			unimplemented(opcode[0]);
			break;
		case 0x15: // DCR  D
			unimplemented(opcode[0]);
			break;
		case 0x16: // MVI  D,d8
			unimplemented(opcode[0]);
			break;
		case 0x17: // RAL
			unimplemented(opcode[0]);
			break;
		case 0x18: // 0x18 ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0x19: // DAD  D
			unimplemented(opcode[0]);
			break;
		case 0x1a: // LDAX D
			unimplemented(opcode[0]);
			break;
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
			break;
		case 0x1f: // RAR
			unimplemented(opcode[0]);
			break;
		case 0x20: // 0x20 ILLEGAL
			unimplemented(opcode[0]);
			break;

		case 0x21: // LXI  H,d16
			unimplemented(opcode[0]);
			break;
		case 0x22: // SHLD a16
			unimplemented(opcode[0]);
			break;
		case 0x23: // INX  H
			unimplemented(opcode[0]);
			break;
		case 0x24: // INR  H
			unimplemented(opcode[0]);
			break;
		case 0x25: // DCR  H
			unimplemented(opcode[0]);
			break;
		case 0x26: // MVI  H,d8
			unimplemented(opcode[0]);
			break;
		case 0x27: // DAA
			unimplemented(opcode[0]);
			break;
		case 0x28: // 0x28 ILLEGAL
			unimplemented(opcode[0]);
			break;
		case 0x29: // DAD  H
			unimplemented(opcode[0]);
			break;
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
			break;
		case 0x2f: // CMA
			unimplemented(opcode[0]);
			break;
		case 0x30: // 0x30 ILLEGAL
			unimplemented(opcode[0]);
			break;

		case 0x31: // LXI  SP d16
			unimplemented(opcode[0]);
			break;
		case 0x32: // STA a16
			unimplemented(opcode[0]);
			break;
		case 0x33: // INX  SP
			unimplemented(opcode[0]);
			break;
		case 0x34: // INR  M
			unimplemented(opcode[0]);
			break;
		case 0x35: // DCR  M
			unimplemented(opcode[0]);
			break;
		case 0x36: // MVI  M,d8
			unimplemented(opcode[0]);
			break;
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
			unimplemented(opcode[0]);
			break;
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
			unimplemented(opcode[0]);
			break;
		case 0x3f: // CMC
			unimplemented(opcode[0]);
			break;

		case 0x40: // MOV B,B
			unimplemented(opcode[0]);
			break;
		case 0x41: // MOV B,C
			unimplemented(opcode[0]);
			break;
		case 0x42: // MOV B,D
			unimplemented(opcode[0]);
			break;
		case 0x43: // MOV B,E
			unimplemented(opcode[0]);
			break;
		case 0x44: // MOV B,H
			unimplemented(opcode[0]);
			break;
		case 0x45: // MOV B,L
			unimplemented(opcode[0]);
			break;
		case 0x46: // MOV B,M
			unimplemented(opcode[0]);
			break;
		case 0x47: // MOV B,A
			unimplemented(opcode[0]);
			break;

		case 0x48: // MOV C,B
			unimplemented(opcode[0]);
			break;
		case 0x49: // MOV C,C
			unimplemented(opcode[0]);
			break;
		case 0x4a: // MOV C,D
			unimplemented(opcode[0]);
			break;
		case 0x4b: // MOV C,E
			unimplemented(opcode[0]);
			break;
		case 0x4c: // MOV C,H
			unimplemented(opcode[0]);
			break;
		case 0x4d: // MOV C,L
			unimplemented(opcode[0]);
			break;
		case 0x4e: // MOV C,M
			unimplemented(opcode[0]);
			break;
		case 0x4f: // MOV C,A
			unimplemented(opcode[0]);
			break;

		case 0x50: // MOV D,B
			unimplemented(opcode[0]);
			break;
		case 0x51: // MOV D,C
			unimplemented(opcode[0]);
			break;
		case 0x52: // MOV D,D
			unimplemented(opcode[0]);
			break;
		case 0x53: // MOV D,E
			unimplemented(opcode[0]);
			break;
		case 0x54: // MOV D,H
			unimplemented(opcode[0]);
			break;
		case 0x55: // MOV D,L
			unimplemented(opcode[0]);
			break;
		case 0x56: // MOV D,M
			unimplemented(opcode[0]);
			break;
		case 0x57: // MOV D,A
			unimplemented(opcode[0]);
			break;

		case 0x58: // MOV E,B
			unimplemented(opcode[0]);
			break;
		case 0x59: // MOV E,C
			unimplemented(opcode[0]);
			break;
		case 0x5a: // MOV E,D
			unimplemented(opcode[0]);
			break;
		case 0x5b: // MOV E,E
			unimplemented(opcode[0]);
			break;
		case 0x5c: // MOV E,H
			unimplemented(opcode[0]);
			break;
		case 0x5d: // MOV E,L
			unimplemented(opcode[0]);
			break;
		case 0x5e: // MOV E,M
			unimplemented(opcode[0]);
			break;
		case 0x5f: // MOV E,A
			unimplemented(opcode[0]);
			break;

		case 0x60: // MOV H,B
			unimplemented(opcode[0]);
			break;
		case 0x61: // MOV H,C
			unimplemented(opcode[0]);
			break;
		case 0x62: // MOV H,D
			unimplemented(opcode[0]);
			break;
		case 0x63: // MOV H,E
			unimplemented(opcode[0]);
			break;
		case 0x64: // MOV H,H
			unimplemented(opcode[0]);
			break;
		case 0x65: // MOV H,L
			unimplemented(opcode[0]);
			break;
		case 0x66: // MOV H,M
			unimplemented(opcode[0]);
			break;
		case 0x67: // MOV H,A
			unimplemented(opcode[0]);
			break;

		case 0x68: // MOV L,B
			unimplemented(opcode[0]);
			break;
		case 0x69: // MOV L,C
			unimplemented(opcode[0]);
			break;
		case 0x6a: // MOV L,D
			unimplemented(opcode[0]);
			break;
		case 0x6b: // MOV L,E
			unimplemented(opcode[0]);
			break;
		case 0x6c: // MOV L,H
			unimplemented(opcode[0]);
			break;
		case 0x6d: // MOV L,L
			unimplemented(opcode[0]);
			break;
		case 0x6e: // MOV L,M
			unimplemented(opcode[0]);
			break;
		case 0x6f: // MOV L,A
			unimplemented(opcode[0]);
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
			break;
		case 0x77: // MOV M,A
			unimplemented(opcode[0]);
			break;
		case 0x78: // MOV A,B
			unimplemented(opcode[0]);
			break;
		case 0x79: // MOV A,C
			unimplemented(opcode[0]);
			break;
		case 0x7a: // MOV A,D
			unimplemented(opcode[0]);
			break;
		case 0x7b: // MOV A,E
			unimplemented(opcode[0]);
			break;
		case 0x7c: // MOV A,H
			unimplemented(opcode[0]);
			break;
		case 0x7d: // MOV A,L
			unimplemented(opcode[0]);
			break;
		case 0x7e: // MOV A,H
			unimplemented(opcode[0]);
			break;
		case 0x7f: // MOV A,H
			unimplemented(opcode[0]);
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
			unimplemented(opcode[0]);
			break;
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
			unimplemented(opcode[0]);
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
			unimplemented(opcode[0]);
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
			unimplemented(opcode[0]);
			break;
		case 0xc2: // JNZ a16
			unimplemented(opcode[0]);
			break;
		case 0xc3: // JMP a16
			unimplemented(opcode[0]);
			break;
		case 0xc4: // CNZ a16
			unimplemented(opcode[0]);
			break;
		case 0xc5: // PUSH B
			unimplemented(opcode[0]);
			break;
		case 0xc6: // ADI d8
			unimplemented(opcode[0]);
			break;
		case 0xc7: // RST 0
			unimplemented(opcode[0]);
			break;
		case 0xc8: // RZ
			unimplemented(opcode[0]);
			break;
		case 0xc9: // RET
			unimplemented(opcode[0]);
			break;
		case 0xca: // JZ a16
			unimplemented(opcode[0]);
			break;
		case 0xcb: // 0xcb ILLEGAL
			unimplemented(opcode[0]);
			break;

		case 0xcc: // CZ a16
			unimplemented(opcode[0]);
			break;
		case 0xcd: // CALL a16
			unimplemented(opcode[0]);
			break;
		case 0xce: // ACI d8
			unimplemented(opcode[0]);
			break;
		case 0xcf: // RST 1
			unimplemented(opcode[0]);
			break;
		case 0xd0: // RNC
			unimplemented(opcode[0]);
			break;
		case 0xd1: // POP D
			unimplemented(opcode[0]);
			break;
		case 0xd2: // JNC a16
			unimplemented(opcode[0]);
			break;
		case 0xd3: // OUT d8
			unimplemented(opcode[0]);
			break;
		case 0xd4: // CNC a16
			unimplemented(opcode[0]);
			break;
		case 0xd5: // PUSH D
			unimplemented(opcode[0]);
			break;
		case 0xd6: // SUI d8
			unimplemented(opcode[0]);
			break;
		case 0xd7: // RST 2
			unimplemented(opcode[0]);
			break;
		case 0xd8: // RC
			unimplemented(opcode[0]);
			break;
		case 0xd9: // 0xd9 ILLEGAL
			unimplemented(opcode[0]);
			break;

		case 0xda: // JC a16
			unimplemented(opcode[0]);
			break;
		case 0xdb: // IN d8
			unimplemented(opcode[0]);
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
			unimplemented(opcode[0]);
			break;
		case 0xe0: // RPO
			unimplemented(opcode[0]);
			break;
		case 0xe1: // POP H
			unimplemented(opcode[0]);
			break;
		case 0xe2: // JPO a16
			unimplemented(opcode[0]);
			break;
		case 0xe3: // XTHL
			unimplemented(opcode[0]);
			break;
		case 0xe4: // CPO a16
			unimplemented(opcode[0]);
			break;
		case 0xe5: // PUSH H
			unimplemented(opcode[0]);
			break;
		case 0xe6: // ANI d8
			unimplemented(opcode[0]);
			break;
		case 0xe7: // RST 4
			unimplemented(opcode[0]);
			break;
		case 0xe8: // RPE
			unimplemented(opcode[0]);
			break;
		case 0xe9: // PCHL
			unimplemented(opcode[0]);
			break;
		case 0xea: // JPE a16
			unimplemented(opcode[0]);
			break;
		case 0xeb: // XCHG
			unimplemented(opcode[0]);
			break;
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
			unimplemented(opcode[0]);
			break;
		case 0xf0: // RP
			unimplemented(opcode[0]);
			break;
		case 0xf1: // POP PSW
			unimplemented(opcode[0]);
			break;
		case 0xf2: // JP a16
			unimplemented(opcode[0]);
			break;
		case 0xf3: // DI
			unimplemented(opcode[0]);
			break;
		case 0xf4: // CP a16
			unimplemented(opcode[0]);
			break;
		case 0xf5: // PUSH PSW
			unimplemented(opcode[0]);
			break;
		case 0xf6: // ORI d8
			unimplemented(opcode[0]);
			break;
		case 0xf7: // RST 6
			unimplemented(opcode[0]);
			break;
		case 0xf8: // RM
			unimplemented(opcode[0]);
			break;
		case 0xf9: // SPHL
			unimplemented(opcode[0]);
			break;
		case 0xfa: // JM a16
			unimplemented(opcode[0]);
			break;
		case 0xfb: // EI
			unimplemented(opcode[0]);
			break;
		case 0xfc: // CM a16
			unimplemented(opcode[0]);
			break;
		case 0xfd: // 0xfd ILLEGAL
			unimplemented(opcode[0]);
			break;

		case 0xfe: // CPI d8
			unimplemented(opcode[0]);
			break;
		case 0xff: // RST 7
			unimplemented(opcode[0]);
			break;
	}

	cpu->registers.pc += bytes;
	return 1;
}