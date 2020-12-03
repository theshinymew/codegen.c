// Alejandro Herrada
// Skyler Bolton

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

symbol* parser(lexeme *lexeme_list);

void insert(int kind, char* name, int value, int level, int address, int mark);
int lookup(int current, int lexlevel);

#endif
