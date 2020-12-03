// Alejandro Herrada
// Skyler Bolton

#ifndef __LEX_H
#define __LEX_H

#define MAX_IDENTIFIER_LENGTH 11
#define MAX_NUMBER_LENGTH 5
#define MAX_TABLE_SIZE 500

// Reserved words declaration
static char *reserved[] = { "odd", "begin", "end", "if", "then", "while", "do", "call",
					 		"const", "var", "procedure", "write", "read", "else" };

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

// Special symbols hash table declaration
token_type ssym[256];

token_type getAlphaTokenType(char *s);
lexeme* lexer(char *inputFile, int flag);

#endif
