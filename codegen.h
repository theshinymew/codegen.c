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

typedef enum
{
	LIT = 1, RTN, LOD, STO, CAL, INC, JMP, JPC, SYS, NEG, ADD, SUB, MUL, DIV, 
	ODD, MOD, EQL, NEQ, LSS, LEQ, GTR, GEQ
} opcode;

static char opcodes[23][4] =
{
	"000", // 0
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

instruction* codegen(symbol *symbol_table, lexeme *lexeme_list, int flag);

#endif