// Alejandro Herrada
// Skyler Bolton

#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"

#define MAX_STACK_HEIGHT 1000
#define MAX_CODE_LENGTH 500

// define instruction structure
typedef struct instruction
{
	int op;
	int r;
	int l;
	int m;
} instruction;

// find base L levels up
int base(int l, int base, int *stack)
{
	while (l > 0)
	{
		base = stack[base];
		l--;
	}
	return base;
}

void virtual machine(instruction *code, bool flag)
{
	// declarations
	int text[MAX_CODE_LENGTH][4] = {0};
	int stack[MAX_STACK_HEIGHT] = {0};
	int ar[MAX_STACK_HEIGHT] = {0};
	int rf[8] = {0};
	int sp = MAX_STACK_HEIGHT;
	int bp = sp - 1;
	int pc = 0;
	instruction ir = {0};


	// open input file
	FILE *fp;
	if((fp = fopen(argv[1], "r")) == NULL)
	{
		printf("Unable to open file :(\n");
		return 1;
	}

	// print out the interpreted assembly language
	// this will help with printing out the instruction names
	char opcodes[22][4] =
	{
		"LIT", // 1
		"RTN", // 2
		"LOD", // 3
		"STO", // 4
		"CAL", // 5
		"INC", // 6
		"JMP", // 7
		"JPC", // 8
		"SYS", // 9
		"NEG", // 10
		"ADD", // 11
		"SUB", // 12
		"MUL", // 13
		"DIV", // 14
		"ODD", // 15
		"MOD", // 16
		"EQL", // 17
		"NEQ", // 18
		"LSS", // 19
		"LEQ", // 20
		"GTR", // 21
		"GEQ"  // 22
	};

	printf("Line OP  R   L   M\n");
	for(int i = 0; fscanf(fp, "%d", &text[i][0]) != EOF; i++)
	{
		// Print line and opcode.
		printf("%-4d %s ", i, opcodes[text[i][0] - 1]);

		// Print line of instructions.
		fscanf(fp, "%d %d %d", &text[i][1], &text[i][2], &text[i][3]);
		printf("%-3d %-3d %-3d\n", text[i][1], text[i][2], text[i][3]);
	}
	printf("\n");

	// begin instruction cycle
	int halt = 1;

	// print initial values
	printf("%28s %-4s %-4s %-4s\n"," ", "pc", "bp", "sp");
	printf("%-28s %-4d %-4d %-4d\n", "Initial Values", pc, bp, sp);
	printf("Registers: ");
	for(int i = 0; i < 8; i++)
		printf("%d ", rf[i]);
	printf("\nStack:\n\n");

	int temp;
	while(halt)
	{
		// fetch cycle
		ir.op = text[pc][0];
		ir.r = text[pc][1];
		ir.l = text[pc][2];
		ir.m = text[pc][3];

		// temp stores the current line
		temp = pc;
		pc++;

		// execute cycle
		switch(ir.op)
		{
		case 1:
			// LIT
			rf[ir.r] = ir.m;
			break;

		case 2:
			// RTN
			ar[bp] = 0; // remove mark from activation record
			sp = bp + 1;
			bp = stack[sp - 2];
			pc = stack[sp - 3];
			break;

		case 3:
			// LOD
			rf[ir.r] = stack[base(ir.l, bp, stack) - ir.m];
			break;

		case 4:
			// STO
			stack[base(ir.l, bp, stack) - ir.m] = rf[ir.r];
			break;

		case 5:
			// CAL
			stack[sp - 1] = base(ir.l, bp, stack); // static link
			stack[sp - 2] = bp; //dynamic link
			stack[sp - 3] = pc; // return address
			bp = sp - 1;
			pc = ir.m;
			ar[bp] = 1; // mark beginning of new activation record
			break;

		case 6:
			// INC
			sp -= ir.m;
			break;

		case 7:
			// JMP
			pc = ir.m;
			break;

		case 8:
			// JPC
			if (rf[ir.r] == 0)
				pc = ir.m;
			break;

		case 9:
			// SYS
			if (ir.m == 1)
			{
				printf("Register %d: %d\n\n", ir.r, rf[ir.r]);
			}
			else if (ir.m == 2)
			{
				printf("Enter an integer: \n");
				scanf("%d", &rf[ir.r]);
			}
			else if(ir.m == 3)
			{
				halt = 0;
			}
			break;

		case 10:
			// NEG
			rf[ir.r] = -rf[ir.r];
			break;

		case 11:
			// ADD
			rf[ir.r] = rf[ir.l] + rf[ir.m];
			break;

		case 12:
			// SUB
			rf[ir.r] = rf[ir.l] - rf[ir.m];
			break;

		case 13:
			// MUL
			rf[ir.r] = rf[ir.l] * rf[ir.m];
			break;

		case 14:
			// DIV
			rf[ir.r] = rf[ir.l] / rf[ir.m];
			break;

		case 15:
			// ODD
			rf[ir.r] = rf[ir.r] % 2;
			break;

		case 16:
			// MOD
			rf[ir.r] = rf[ir.l] % rf[ir.m];
			break;

		case 17:
			// EQL
			rf[ir.r] = rf[ir.l] == rf[ir.m];
			break;

		case 18:
			// NEQ
			rf[ir.r] = rf[ir.l] != rf[ir.m];
			break;

		case 19:
			// LSS
			rf[ir.r] = rf[ir.l] < rf[ir.m];
			break;

		case 20:
			// LEQ
			rf[ir.r] = rf[ir.l] <= rf[ir.m];
			break;

		case 21:
			// GTR
			rf[ir.r] = rf[ir.l] > rf[ir.m];
			break;

		case 22:
			// GEQ
			rf[ir.r] = rf[ir.l] >= rf[ir.m];
			break;

		default:
			break;
		}

		// print the stack after executing each instruction
		printf("%2d %s %2d %2d %2d", temp, opcodes[ir.op - 1], ir.r, ir.l, ir.m);
		printf("%13s %-4d %-4d %-4d\n", " ", pc, bp, sp);
		printf("Registers: ");
		for(int i = 0; i < 8; i++)
			printf("%d ", rf[i]);
		printf("\nStack: ");

		// print the stack
		for(int i = MAX_STACK_HEIGHT - 1; i >= sp; i--)
		{
			if(ar[i])
			{
				printf("| ");
			}
			printf("%d ", stack[i]);
		}
		printf("\n\n");
	}
}