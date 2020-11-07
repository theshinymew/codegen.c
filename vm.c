// Alejandro Herrada
// Skyler Bolton

#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"

#define MAX_STACK_HEIGHT 1000
#define MAX_CODE_LENGTH 500

// define instruction structure
//typedef struct instruction
//{
//	int op;
//	int r;
//	int l;
//	int m;
//} instruction;

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
	
}
