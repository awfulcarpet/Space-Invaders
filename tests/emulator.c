#include <stdio.h>
#include <stdlib.h>
#include "../src/cpu.h"
#include "../src/dissasemble.h"

int
main(void) {
	struct CPU cpu = {0};
	FILE *f = fopen("cpudiag.bin", "r");
	if (f == NULL) {
		perror("failed to open rom");
		return 1;
	}

	fseek(f, 0, SEEK_END);
	int len = ftell(f);
	fseek(f, 0, SEEK_SET);
	/*cpu.ram = calloc(len + 0x100, sizeof(uint8_t));*/
	cpu.ram = calloc(0xffff, 1);

	fread(&cpu.ram[0x100], sizeof(uint8_t), len, f);
	fclose(f);

	/*cpu.ram[0] = 0xc3;*/
	/*cpu.ram[1] = 0;*/
	/*cpu.ram[2] = 0x01;*/
	cpu.pc = 0x100;

	cpu.ram[368] = 0x7;

	cpu.ram[0x59c] = 0xc3;
	cpu.ram[0x59d] = 0xc2;
	cpu.ram[0x59e] = 0x05;

	printf("starting tests\n");
	while (1) {
		get_opname(cpu.ram, cpu.pc);
		emulate(&cpu);
		print_cpu_state(&cpu, 0);
	}
}
