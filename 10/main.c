
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLEN 128
#define MAXPATH 128
#define MAXCHILDREN 128

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
typedef int bool;
#define true 1
#define false 0

typedef enum OPS {
  NOOP,
  ADDX_CYC_1,
  ADDX_CYC_2,
} OPS;

typedef struct CPU {
    int reg_x;
    int pc;
} CPU;

typedef struct Instruction {
    OPS opcode;
    int param1;
} Instruction;

#define Vec_Type Instruction
#include "../common/vec.h"

CPU CPU_create() {
    CPU c = {.pc = 0, .reg_x = 1 };
    return c;
}

bool CPU_execute(CPU* cpu, Vec instructions) {
    if( (size_t)cpu->pc == instructions.size ) {
        return false;
    }

    Instruction inst = instructions.items[cpu->pc];

    switch(inst.opcode)
    {
        case NOOP:
            //printf("CPU %d: NOOP\n", cpu->pc);
            break;
        case ADDX_CYC_1:
            //printf("CPU %d: ADDX(NOOP)\n", cpu->pc);
            break;
        case ADDX_CYC_2:
            cpu->reg_x += inst.param1;
            //printf("CPU %d: ADDX %d : %d\n", cpu->pc, inst.param1, cpu->reg_x);
            break;
    }

    cpu->pc++;

    return true;
}

void problem1(FILE *f)
{
    char buffer[MAXLEN];
    memset(buffer, '\0', MAXLEN * sizeof(char));

    CPU cpu = CPU_create();
    Vec program = Vec_create();

    while (fgets(buffer, MAXLEN, f))
    {
        char direction;
        int distance;
        size_t length = strlen(buffer);
        if( buffer[length - 1] == '\n' ) {
            buffer[length - 1] = '\0';
            length = strlen(buffer);
        }   

    
        char* op = strtok(buffer, " ");
        char* sparam1 = strtok(NULL, " ");

        Instruction i;
        if( sparam1 != NULL && strlen(sparam1) != 0 ) i.param1 = atoi(sparam1);
        if( strcmp(op, "noop") == 0 ) {
            i.opcode = NOOP;
            Vec_push_back(&program, i);
        }
        else if( strcmp(op, "addx") == 0 ) {
            i.opcode = ADDX_CYC_1;
            Vec_push_back(&program, i);
            i.opcode = ADDX_CYC_2;
            Vec_push_back(&program, i);
        }
    }

    int sigStrengthSum = 0;
    while( CPU_execute(&cpu, program) ) {
        // problem 1
        // The problem describes cycles weird, so we're off by one
        // as far as the result calculation goes..
        int pc = cpu.pc + 1;
        if( pc == 20 ||
            (pc - 20) % 40 == 0) {
            int sigStrength = pc * cpu.reg_x;
            sigStrengthSum += sigStrength;
            // printf("Cycle %d: X: %d Strength: %d\n", pc, cpu.reg_x, sigStrength);
        }

        // problem 2 - Render the CRT
        // This is offset by one pixel to the left, but probably because of the hack above
        // Either way, problem solved, interesting one :)
        int crtPos = (cpu.pc) % 40;
        if( abs(cpu.reg_x - crtPos) <=1 ) {
            printf("#");
        } else {
            printf(".");
        }
        if( crtPos == 0 ) {
            printf("\n");
        }
    }
    printf("Signal Strength Sum: %d\n", sigStrengthSum);

    Vec_destroy(&program);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("USAGE: %s input.txt\n", argv[0]);
        return 1;
    }

    const char *inputFilePath = argv[1];

    FILE *inputFile = fopen(inputFilePath, "r");
    if (!inputFile)
    {
        printf("Error: Failed to open file: %s", inputFilePath);
        return 1;
    }

    problem1(inputFile);
    // fseek(inputFile, 0, SEEK_SET);
    // problem2(inputFile);
    fclose(inputFile);

    return 0;
}
