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

symbol* parser(lexeme *list, int flag);

void insert(int kind, char* name, int value, int level, int address, int mark);
int lookup(char* name);

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
