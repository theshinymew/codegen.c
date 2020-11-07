#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lex.h"

#define TOKEN list[current].token // this is a pain to type

lexeme *list;
symbol *table;

// Global variables to keep track of current token, symbol table capacity, and variable counter
int current, symcount, varcount = 0;
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
}

// Symbol table lookup function
// Returns identifier index in symbol table, or -1 if it doesn't exist
int lookup(char* name)
{
    for(int i = 0; i < symcount; i++)
    {
        if(!strcasecmp(name, table[i].name))
            return i;
    }
    return -1;
}

void PROGRAM()
{
    BLOCK();
    if(TOKEN != periodsym)
        printf("ERROR: Expected period terminator\n");
}

void BLOCK()
{
    CONST_DECLARATION();
    VAR_DECLARATION();
    STATEMENT();   
}

void CONST_DECLARATION()
{
    if(TOKEN == constsym)
    {
        do
        {
            current++;
            if(TOKEN != identsym)
            {
                printf("ERROR: const, var, procedure must be followed by identifier\n");
                exit(1);
            }
            
            // save ident name
            name = list[current].name;
            if(lookup(name) != -1)
            {
                printf("ERROR: identifier has already been declared\n");
                exit(1);
            }

            current++;
            if(TOKEN != eqsym)
            {
                printf("ERROR: identifier must be followed by '='\n");
                exit(1);
            }
            
            current++;
            if(TOKEN != numbersym)
            {
                printf("ERROR: '=' must be followed by a number\n");
                exit(1);
            }
            
            // Add to symbol table.
            insert(1, name, list[current].value, 0, 0, 0);
            symcount++;
            current++;
        }
        while(TOKEN == commasym);
        
        if(TOKEN != semicolonsym)
        {
            printf("ERROR: declaration must end with ';'\n");
            exit(1);
        }
        current++;
    }
}

void VAR_DECLARATION()
{
    if(TOKEN == varsym)
    {
        do
        {
            current++;
            if(TOKEN != identsym)
            {
                printf("ERROR: const, var, procedure must be followed by identifier\n");
                exit(1);
            }

            // save ident name
            name = list[current].name;
            if(lookup(name) != -1)
            {
                printf("ERROR: identifier has already been declared\n");
                exit(1);
            }

            // Add to symbol table.
            insert(2, name, 0, 0, varcount + 2, 0);
            symcount++;
            varcount++;
            current++;
        } 
        while(TOKEN == commasym);

        if(TOKEN != semicolonsym)
        {
            printf("ERROR: declaration must end with ';'\n");
            exit(1);
        }
        current++;
    }
}

void STATEMENT()
{
    if(TOKEN == identsym)
    {
        name = list[current].name;
        if(lookup(name) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(1);
        }

        if(table[lookup(name)].kind != 2)
        {
            printf("ERROR: assignment to constant is not allowed\n");
            exit(1);
        }

        current++;
        if(TOKEN != becomessym)
        {
            printf("ERROR: assignment operator expected\n");
            exit(1);
        }

        current++;
        EXPRESSION();
        return;
    }

    if(TOKEN == beginsym)
    {
        do
        {
            current++;
            STATEMENT();
        }
        while(TOKEN == semicolonsym);

        if(TOKEN != endsym)
        {
            printf("ERROR: begin must be closed with end\n");
            exit(1);
        }

        current++;
        return;        
    }

    if(TOKEN == ifsym)
    {
        current++;
        CONDITION();
        
        if(TOKEN != thensym)
        {
            printf("ERROR: if condition must be followed by then\n");
            exit(1);
        }

        current++;
        STATEMENT();
        return;
    }

    if(TOKEN == whilesym)
    {
        current++;
        CONDITION();

        if(TOKEN != dosym)
        {
            printf("ERROR: while condition must be followed by do\n");
            exit(1);
        }

        current++;
        STATEMENT();
        return;        
    }

    if(TOKEN == readsym)
    {
        current++;
        if(TOKEN != identsym)
        {
            printf("ERROR: read statement must be followed by identifier\n");
            exit(1);
        }

        name = list[current].name;
        if(lookup(name) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(1);
        }

        if(table[lookup(name)].kind != 2)
        {
            printf("ERROR: assignment to constant is not allowed\n");
            exit(1);
        }

        current++;
        return;
    }

    if(TOKEN == writesym)
    {
        current++;
        if(TOKEN != identsym)
        {
            printf("ERROR: write statement must be followed by identifier\n");
            exit(1);
        }

        name = list[current].name;
        if(lookup(name) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(1);
        }

        current++;
        return;
    }
}

void CONDITION()
{
    if(TOKEN == oddsym)
    {
        current++;
        EXPRESSION();
    }
    else
    {
        EXPRESSION();

        if(TOKEN != eqsym || TOKEN != neqsym || TOKEN != lessym || TOKEN != leqsym || TOKEN != gtrsym || TOKEN != geqsym)
        {
            printf("ERROR: relational operator expected\n");
            exit(1);
        }

        current++;
        EXPRESSION();
    }
}

void EXPRESSION()
{
    if(TOKEN == plussym || TOKEN == minussym)
    {
        current++;
    }
    TERM();

    while(TOKEN == plussym || TOKEN == minussym)
    {
        current++;
        TERM();
    }
        
}

void TERM()
{
    FACTOR();

    while(TOKEN == multsym || TOKEN == slashsym)
    {
        current++;
        FACTOR();
    }
}

void FACTOR()
{
    if(TOKEN == identsym)
    {
        name = list[current].name;
        if(lookup(name) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(1);
        }

        current++;
    }
    else if(TOKEN == numbersym)
    {
        current++;
    }
    else if(TOKEN == lparentsym)
    {
        current++;
        EXPRESSION();

        if(TOKEN != rparentsym)
        {
            printf("ERROR: right parenthesis missing\n");
            exit(1);
        }

        current++;
    }
    else
    {
        printf("ERROR: the preceding factor cannot begin with this symbol\n");
        exit(1);
    }
    
}

symbol* parser(lexeme *lexList, int flag)
{
    list = lexList;
    table = malloc(sizeof(symbol) * MAX_TABLE_SIZE);
    
    PROGRAM();
    return table;
}