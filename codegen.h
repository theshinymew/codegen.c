#ifndef __CODEGEN_H
#define __CODEGEN_H

#include "lex.h"
#include "parser.h"

typedef struct instruction
{
	int op;
	int r;
	int l;
	int m;
} instruction;

instruction* codegen(symbol *table, lexeme *list);

#endif