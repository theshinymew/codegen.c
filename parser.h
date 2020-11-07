#ifndef __PARSER_H
#define __PARSER_H

#include "lex.h"

typedef struct symbol
{
	int kind;
	char name[12];
	int val;
	int level;
	int addr;
	int mark;
} symbol;

symbol table[MAX_TABLE_SIZE];

symbol* parser(lexeme *list, int flag);

void PROGRAM();
void BLOCK();
void CONST_DECLARATION();
void VAR_DECLARATION();
void STATEMENT();
void CONDITION();
void EXPRESSION();
void TERM();
void FACTOR();

#endif
