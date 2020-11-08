// Alejandro Herrada
// Skyler Bolton

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lex.h"

lexeme *list;
symbol *table;

// Global variables to keep track of current token, symbol table capacity, and variable counter
int current = 0;
int symcount = 0;
int varcount = 0;
char *name;

// Symbol table insert function
void insert(int kind, char* name, int value, int level, int address, int mark)
{
    table[symcount].kind = kind;
    strcpy(table[symcount].name, name);
    table[symcount].val = value;
    table[symcount].level = level;
    table[symcount].addr = address;
    table[symcount].mark = mark;

    symcount++;
}

// Symbol table lookup function
// Returns identifier index in symbol table, or -1 if it doesn't exist
int lookup(char* name)
{
    for(int i = 0; i < symcount; i++)
    {
        if(!strcmp(name, table[i].name))
            return i;
    }
    return -1;
}

void PROGRAM()
{
    BLOCK();

    if(list[current].token != periodsym)
    {
        printf("ERROR: period expected\n");
        exit(EXIT_FAILURE);
    }
}

void BLOCK()
{

    CONST_DECLARATION();
    VAR_DECLARATION();
    STATEMENT();
}

void CONST_DECLARATION()
{
    if(list[current].token == constsym)
    {
        do
        {
            current++;
            if(list[current].token != identsym)
            {
                printf("ERROR: const, var, procedure must be followed by identifier\n");
                exit(EXIT_FAILURE);
            }

            // save ident name
            name = list[current].name;
            if(lookup(name) != -1)
            {
                printf("ERROR: identifier has already been declared\n");
                exit(EXIT_FAILURE);
            }

            current++;
            if(list[current].token != eqsym)
            {
                printf("ERROR: constant declaration must be followed by '='\n");
                exit(EXIT_FAILURE);
            }

            current++;
            if(list[current].token != numbersym)
            {
                printf("ERROR: '=' must be followed by a number\n");
                exit(EXIT_FAILURE);
            }

            // Add to symbol table.
            insert(1, name, list[current].value, 0, 0, 0);
            current++;
        }
        while(list[current].token == commasym);

        if(list[current].token != semicolonsym)
        {
            printf("ERROR: declaration must end with ';'\n");
            exit(EXIT_FAILURE);
        }
        current++;
    }
}

void VAR_DECLARATION()
{

    if(list[current].token == varsym)
    {
        
        do
        {
            current++;
            varcount++;
            if(list[current].token != identsym)
            {
                printf("ERROR: const, var, procedure must be followed by identifier\n");
                exit(EXIT_FAILURE);
            }

            // save ident name
            name = list[current].name;
            if(lookup(name) != -1)
            {
                printf("ERROR: identifier has already been declared\n");
                exit(EXIT_FAILURE);
            }

            // Add to symbol table.
            insert(2, name, 0, 0, varcount + 2, 0);
            current++;
        }
        while(list[current].token == commasym);

        if(list[current].token != semicolonsym)
        {
            printf("ERROR: declaration must end with ';'\n");
            exit(EXIT_FAILURE);
        }
        current++;
    }
}

void STATEMENT()
{
    if(list[current].token == identsym)
    {
        name = list[current].name;
        if(lookup(name) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(EXIT_FAILURE);
        }

        if(table[lookup(name)].kind != 2)
        {
            printf("ERROR: assignment to constant is not allowed\n");
            exit(EXIT_FAILURE);
        }

        current++;
        if(list[current].token != becomessym)
        {
            printf("ERROR: ':=' expected\n");
            exit(EXIT_FAILURE);
        }

        current++;
        EXPRESSION();
        return;
    }

    if(list[current].token == beginsym)
    {
        do
        {
            current++;
            STATEMENT();
        }
        while(list[current].token == semicolonsym);

        if(list[current].token != endsym)
        {
            printf("ERROR: begin must be closed with end\n");
            exit(EXIT_FAILURE);
        }

        current++;
        return;
    }

    if(list[current].token == ifsym)
    {
        current++;
        CONDITION();

        if(list[current].token != thensym)
        {
            printf("ERROR: if condition must be followed by then\n");
            exit(EXIT_FAILURE);
        }

        current++;
        STATEMENT();
        return;
    }

    if(list[current].token == whilesym)
    {
        current++;
        CONDITION();

        if(list[current].token != dosym)
        {
            printf("ERROR: while condition must be followed by do\n");
            exit(EXIT_FAILURE);
        }

        current++;
        STATEMENT();
        return;
    }

    if(list[current].token == readsym)
    {
        current++;
        if(list[current].token != identsym)
        {
            printf("ERROR: read statement must be followed by identifier\n");
            exit(EXIT_FAILURE);
        }

        name = list[current].name;
        if(lookup(name) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(EXIT_FAILURE);
        }

        if(table[lookup(name)].kind != 2)
        {
            printf("ERROR: assignment to constant is not allowed\n");
            exit(EXIT_FAILURE);
        }

        current++;
        return;
    }

    if(list[current].token == writesym)
    {
        current++;
        if(list[current].token != identsym)
        {
            printf("ERROR: write statement must be followed by identifier\n");
            exit(EXIT_FAILURE);
        }

        name = list[current].name;
        if(lookup(name) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(EXIT_FAILURE);
        }

        current++;
        return;
    }
}

void CONDITION()
{
    if(list[current].token == oddsym)
    {
        current++;
        EXPRESSION();
    }
    else
    {
        EXPRESSION();

        if(list[current].token != eqsym && list[current].token != neqsym && list[current].token != lessym &&
           list[current].token != leqsym && list[current].token != gtrsym && list[current].token != geqsym)
        {
            printf("ERROR: relational operator expected\n");
            exit(EXIT_FAILURE);
        }

        current++;
        EXPRESSION();
    }
}

void EXPRESSION()
{
    if(list[current].token == plussym || list[current].token == minussym)
    {
        current++;
    }
    TERM();

    while(list[current].token == plussym || list[current].token == minussym)
    {
        current++;
        TERM();
    }

}

void TERM()
{
    FACTOR();

    while(list[current].token == multsym || list[current].token == slashsym)
    {
        current++;
        FACTOR();
    }
}

void FACTOR()
{
    if(list[current].token == identsym)
    {
        name = list[current].name;
        if(lookup(name) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(EXIT_FAILURE);
        }

        current++;
    }
    else if(list[current].token == numbersym)
    {
        current++;
    }
    else if(list[current].token == lparentsym)
    {
        current++;
        EXPRESSION();

        if(list[current].token != rparentsym)
        {
            printf("ERROR: right parenthesis missing\n");
            exit(EXIT_FAILURE);
        }

        current++;
    }
    else
    {
        printf("ERROR: the preceding factor cannot begin with this symbol\n");
        exit(EXIT_FAILURE);
    }

}

symbol* parser(lexeme *lexeme_list)
{
    list = lexeme_list;
    table = malloc(sizeof(symbol) * MAX_TABLE_SIZE);

    PROGRAM();
    return table;
}
