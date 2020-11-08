// Alejandro Herrada
// Skyler Bolton

#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"

#define MAX_STACK_HEIGHT 1000

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

void vm(instruction *code, int flag)
{
	// declarations
	int stack[MAX_STACK_HEIGHT] = {0};
	int ar[MAX_STACK_HEIGHT] = {0};
	int rf[8] = {0};
	int sp = MAX_STACK_HEIGHT;
	int bp = sp - 1;
	int pc = 0;
	instruction ir;

	// begin instruction cycle
	int halt = 1;

	if(flag)
	{
		// print initial values
		printf("%28s %-4s %-4s %-4s\n"," ", "pc", "bp", "sp");
		printf("%-28s %-4d %-4d %-4d\n", "Initial Values", pc, bp, sp);
		printf("Registers: ");
		for(int i = 0; i < 8; i++)
			printf("%d ", rf[i]);
		printf("\nStack:\n\n");
	}

	int temp;
	while(halt)
	{
		// fetch cycle
		ir.op = code[pc].op;
		ir.r = code[pc].r;
		ir.l = code[pc].l;
		ir.m = code[pc].m;

		// temp stores the current line
		temp = pc;
		pc++;

		// execute cycle
		switch(ir.op)
		{
		case LIT:
			rf[ir.r] = ir.m;
			break;

		case RTN:
			ar[bp] = 0; // remove mark from activation record
			sp = bp + 1;
			bp = stack[sp - 2];
			pc = stack[sp - 3];
			break;

		case LOD:
			rf[ir.r] = stack[base(ir.l, bp, stack) - ir.m];
			break;

		case STO:
			stack[base(ir.l, bp, stack) - ir.m] = rf[ir.r];
			break;

		case CAL:
			stack[sp - 1] = base(ir.l, bp, stack); // static link
			stack[sp - 2] = bp; //dynamic link
			stack[sp - 3] = pc; // return address
			bp = sp - 1;
			pc = ir.m;
			ar[bp] = 1; // mark beginning of new activation record
			break;

		case INC:
			sp -= ir.m;
			break;

		case JMP:
			pc = ir.m;
			break;

		case JPC:
			if (rf[ir.r] == 0)
				pc = ir.m;
			break;

		case SYS:
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

		case NEG:
			rf[ir.r] = -rf[ir.r];
			break;

		case ADD:
			rf[ir.r] = rf[ir.l] + rf[ir.m];
			break;

		case SUB:
			rf[ir.r] = rf[ir.l] - rf[ir.m];
			break;

		case MUL:
			rf[ir.r] = rf[ir.l] * rf[ir.m];
			break;

		case DIV:
			rf[ir.r] = rf[ir.l] / rf[ir.m];
			break;

		case ODD:
			rf[ir.r] = rf[ir.r] % 2;
			break;

		case MOD:
			rf[ir.r] = rf[ir.l] % rf[ir.m];
			break;

		case EQL:
			rf[ir.r] = rf[ir.l] == rf[ir.m];
			break;

		case NEQ:
			rf[ir.r] = rf[ir.l] != rf[ir.m];
			break;

		case LSS:
			rf[ir.r] = rf[ir.l] < rf[ir.m];
			break;

		case LEQ:
			rf[ir.r] = rf[ir.l] <= rf[ir.m];
			break;

		case GTR:
			rf[ir.r] = rf[ir.l] > rf[ir.m];
			break;

		case GEQ:
			rf[ir.r] = rf[ir.l] >= rf[ir.m];
			break;

		default:
			break;
		}

		if(flag)
		{
			// print the stack after executing each instruction
			printf("%2d %s %2d %2d %2d", temp, opcodes[ir.op], ir.r, ir.l, ir.m);
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
}
