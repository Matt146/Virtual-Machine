#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

typedef unsigned long long int uint64_t;
typedef long long int int64_tb;


#define REGISTER_COUNT 3
#define OUTFILE "product.outfile"
const uint64_t MEMSIZE = 100000000; 

const uint64_t program[] = {
#include "program.nodex"
};

typedef struct Cpu {
	bool running;
	uint64_t pc;
	int64_tb reg1;
	int64_tb reg2;
	int64_tb reg3;
	uint64_t instruction_type;
	uint64_t* memory;
	int64_t imm;
} cpu_t;

cpu_t* init_cpu() {
	cpu_t* cpu = (cpu_t*)malloc(sizeof(cpu_t));
	cpu->memory = (uint64_t*)malloc(sizeof(uint64_t) * MEMSIZE);
	for (uint64_t i = 0; i < MEMSIZE; i++) {
		cpu->memory[i] = 0;
	}
	cpu->pc = 0;
	cpu->reg1 = 0;
	cpu->reg2 = 0;
	cpu->reg3 = 0;
	cpu->running = true;
	return cpu;
}

uint64_t fetch(uint64_t instruction_num) {
	return program[instruction_num];
}

void decode(cpu_t* cpu, uint64_t instr) {
	cpu->instruction_type = (instr & 0xf000) >> 12;
	cpu->reg1     = (instr & 0xf00 ) >>  8;
	cpu->reg2     = (instr & 0xf0  ) >>  4;
	cpu->reg3     = (instr & 0xf   );
	cpu->imm      = (instr & 0xff  );
}

void execute(cpu_t* cpu) {
	switch(cpu->instruction_type) {
		case 0:
			// halt
			cpu->running = false;
			break;
		case 1:
			// add
			printf("add\n");
			cpu->reg1 = cpu->reg2 + cpu->reg3;
			break;
		case 2:
			// mov1
			printf("mov1");
			cpu->reg1 = cpu->imm;
			break;
		case 3:
			// mov2
			cpu->reg2 = cpu->imm;
			break;
		case 4:
			// mov3
			cpu->reg3 = cpu->imm;
		case 5:
			// load
			cpu->memory[cpu->imm] = cpu->reg1;
		case 6:
			// store
			cpu->reg1 = cpu->memory[cpu->imm];
		case 7:
			// mov12
			cpu->reg1 = cpu->reg2;
		case 8:
			// mov13
			cpu->reg1 = cpu->reg3;
		case 9:
			// mov21
			cpu->reg2 = cpu->reg1;
		case 10:
			// mov31
			cpu->reg3 = cpu->reg1;
	}
}

void create_outfile(cpu_t* cpu) {
        FILE* fp = fopen(OUTFILE, "w");
        fprintf(fp, "[Registers]\n");
        fprintf(fp, "reg1: %lld\n", cpu->reg1);
        fprintf(fp, "reg2: %lld\n", cpu->reg2);
        fprintf(fp, "reg3: %lld\n", cpu->reg3);
        fprintf(fp, "[Memory]\n");
        for (uint64_t i = 0 ; i < MEMSIZE; i++) {
                fprintf(fp, "%lld: %lld\n", i, cpu->memory[i]);
        }
	fprintf(fp, "[FEOF]\n");
        fclose(fp);
}

void run() {
	cpu_t* cpu = init_cpu();
	while (cpu->running) {
		uint64_t instruction = fetch(cpu->pc);
		decode(cpu, instruction);
		execute(cpu);
		cpu->pc++;
	}
	create_outfile(cpu);
}

int main() {
	run();
	return 0; 
}
