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
<<<<<<< HEAD

} symbol;

symbol* parser(lexeme *list, int flag);
=======
} symbol;

symbol* parser(lexeme *list, int flag, int j);
>>>>>>> 30bb9c4591662e0a4d9a559c4ce21faa42dfd5a5

#endif
