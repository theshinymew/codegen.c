#ifndef __LEX_H
#define __LEX_H

#include <stdio.h>

//TODO: typedef declarations here

// Token declaration
typedef enum
{
	nulsym = 1, identsym, numbersym, plussym, minussym,	multsym, slashsym,
	oddsym, eqsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym,
	rparentsym, commasym, semicolonsym,	periodsym, becomessym, beginsym, endsym,
	ifsym, thensym,	whilesym, dosym, callsym, constsym, varsym, procsym,
	writesym, readsym, elsesym, commentsym, endcommentsym
} token_type;

// Error type declaration
typedef enum
{
	invalidid = 1, numtoolong, idtoolong, invalidsym
} error_type;

// Lexeme struct
typedef struct lexeme
{
	char* name;
	int value;
	token_type token;
	error_type error;
} lexeme;

lexeme* lexer(FILE *fp, int flag);

#endif
