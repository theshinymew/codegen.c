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
int lookup(int current, int lexlevel)
{
    for(int i = symcount - 1; i >= 0; i--)
    {
        if((!strcmp(TNAME, table[i].name)) && table[i].level <= lexlevel && table[i].mark == 0)
            return i;
    }
    return -1;
}

void PROGRAM()
{
    char main[] = "main";
    insert(PROC, main, 0, 0, 0, 0);
    
    BLOCK(0);

    if(TOKEN != periodsym)
    {
        printf("ERROR: period expected\n");
        exit(EXIT_FAILURE);
    }
}

void BLOCK(int lexlevel)
{
    int numSymbols = 0;
    numSymbols += CONST_DECLARATION(lexlevel);
    numSymbols += VAR_DECLARATION(lexlevel);
    numSymbols += PROCEDURE_DECLARATION(lexlevel);
    STATEMENT(lexlevel);

    // mark the last numsymbols number of unmarked symbols
    while(numSymbols > 0)
    {
        if(!table[numSymbols].mark)
        {
            table[numSymbols].mark = 1;
        }        
        numSymbols--;
    }
}

int CONST_DECLARATION(int lexlevel)
{
    int numConsts = 0;
    if(TOKEN == constsym)
    {
        do
        {
            current++;
            if(TOKEN != identsym)
            {
                printf("ERROR: const, var, procedure must be followed by identifier\n");
                exit(EXIT_FAILURE);
            }

            // save ident name
            int temp = lookup(current, lexlevel);
            if(temp != -1)
            {
                printf("ERROR: identifier has already been declared\n");
                exit(EXIT_FAILURE);
            }
            else if (table[temp].level == lexlevel)
            {
                printf("ERROR: identifier has already been declared\n");
                exit(EXIT_FAILURE);
            }

            current++;
            if(TOKEN != eqsym)
            {
                printf("ERROR: constant declaration must be followed by '='\n");
                exit(EXIT_FAILURE);
            }

            current++;
            if(TOKEN != numbersym)
            {
                printf("ERROR: '=' must be followed by a number\n");
                exit(EXIT_FAILURE);
            }

            // Add to symbol table.
            insert(CONST, TNAME, list[current].value, 0, 0, 0);
            numConsts++;
            current++;
        }
        while(TOKEN == commasym);

        if(TOKEN != semicolonsym)
        {
            printf("ERROR: declaration must end with ';'\n");
            exit(EXIT_FAILURE);
        }
        current++;
    }
    return numConsts;
}

int VAR_DECLARATION(int lexlevel)
{
    int numVars = 0;
    if(TOKEN == varsym)
    {
        do
        {
            current++;
            varcount++;
            if(TOKEN != identsym)
            {
                printf("ERROR: const, var, procedure must be followed by identifier\n");
                exit(EXIT_FAILURE);
            }

            int temp = lookup(current, lexlevel);
            if(temp != -1 && table[temp].level == lexlevel)
            {
                printf("ERROR: identifier has already been declared\n");
                exit(EXIT_FAILURE);
            }
            
            // Add to symbol table.
            insert(VAR, TNAME, 0, 0, varcount + 2, 0);
            numVars++;
            current++;
        }
        while(TOKEN == commasym);

        if(TOKEN != semicolonsym)
        {
            printf("ERROR: declaration must end with ';'\n");
            exit(EXIT_FAILURE);
        }
        current++;
    }
    return numVars;
}

int PROCEDURE_DECLARATION(int lexlevel)
{
    int numProcedures = 0;
    if(TOKEN == procsym)
    {
        do
        {
            current++;
            if(TOKEN != identsym)
            {
                printf("ERROR: const, var, procedure must be followed by identifier\n");
                exit(EXIT_FAILURE);
            }

            int temp = lookup(current, lexlevel);
            if(temp != -1)
            {
                printf("ERROR: identifier has already been declared\n");
                exit(EXIT_FAILURE);
            }
            else if (table[temp].level == lexlevel)
            {
                printf("ERROR: identifier has already been declared\n");
                exit(EXIT_FAILURE);
            }

            insert(PROC, TNAME, 0, lexlevel, 0, 0);
            numProcedures++; // I had this AFTER block earlier-- reason?
            current++;

            if(TOKEN != semicolonsym)
            {
                printf("ERROR: declaration must end with ';'\n");
                exit(EXIT_FAILURE);
            }

            current++;
            BLOCK(lexlevel + 1);

            if(TOKEN != semicolonsym)
            {
                printf("ERROR: declaration must end with ';'\n");
                exit(EXIT_FAILURE);
            }
            current++;
        } while(TOKEN == procsym);
    }
    return numProcedures;
}

void STATEMENT(int lexlevel)
{
    if(TOKEN == identsym)
    {
        name = TNAME;

        if(lookup(current, lexlevel) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(EXIT_FAILURE);
        }

        if(table[lookup(current, lexlevel)].kind != 2)
        {
            printf("ERROR: assignment to constant is not allowed\n");
            exit(EXIT_FAILURE);
        }

        current++;
        if(TOKEN != becomessym)
        {
            printf("ERROR: ':=' expected\n");
            exit(EXIT_FAILURE);
        }

        current++;
        EXPRESSION(lexlevel);
        return;
    }

    if(TOKEN == callsym)
    {
        current++;
        name = TNAME;

        if(lookup(current, lexlevel) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(EXIT_FAILURE);
        }

        current++;

        return;
    }

    if(TOKEN == beginsym)
    {
        do
        {
            current++;
            STATEMENT(lexlevel);
        }
        while(TOKEN == semicolonsym);

        if(TOKEN != endsym)
        {
            printf("ERROR: begin must be closed with end\n");
            exit(EXIT_FAILURE);
        }

        current++;
        return;
    }

    if(TOKEN == ifsym)
    {
        current++;
        CONDITION(lexlevel);

        if(TOKEN != thensym)
        {
            printf("ERROR: if condition must be followed by then\n");
            exit(EXIT_FAILURE);
        }

        current++;
        STATEMENT(lexlevel);
        return;
    }

    if(TOKEN == whilesym)
    {
        current++;
        CONDITION(lexlevel);

        if(TOKEN != dosym)
        {
            printf("ERROR: while condition must be followed by do\n");
            exit(EXIT_FAILURE);
        }

        current++;
        STATEMENT(lexlevel);
        return;
    }

    if(TOKEN == readsym)
    {
        current++;
        if(TOKEN != identsym)
        {
            printf("ERROR: read statement must be followed by identifier\n");
            exit(EXIT_FAILURE);
        }

        name = TNAME;
        if(lookup(current, lexlevel) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(EXIT_FAILURE);
        }

        if(table[lookup(current, lexlevel)].kind != 2)
        {
            printf("ERROR: assignment to constant is not allowed\n");
            exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
        }

        name = TNAME;
        if(lookup(current, lexlevel) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(EXIT_FAILURE);
        }

        current++;
        return;
    }
}

void CONDITION(int lexlevel)
{
    if(TOKEN == oddsym)
    {
        current++;
        EXPRESSION(lexlevel);
    }
    else
    {
        EXPRESSION(lexlevel);

        if(TOKEN != eqsym && TOKEN != neqsym && TOKEN != lessym &&
           TOKEN != leqsym && TOKEN != gtrsym && TOKEN != geqsym)
        {
            printf("ERROR: relational operator expected\n");
            exit(EXIT_FAILURE);
        }

        current++;
        EXPRESSION(lexlevel);
    }
}

void EXPRESSION(int lexlevel)
{
    if(TOKEN == plussym || TOKEN == minussym)
    {
        current++;
    }
    TERM(lexlevel);

    while(TOKEN == plussym || TOKEN == minussym)
    {
        current++;
        TERM(lexlevel);
    }

}

void TERM(int lexlevel)
{
    FACTOR(lexlevel);

    while(TOKEN == multsym || TOKEN == slashsym)
    {
        current++;
        FACTOR(lexlevel);
    }
}

// TODO
void FACTOR(int lexlevel)
{
    if(TOKEN == identsym)
    {
        name = TNAME;
        // search the symbol table working backwards
        // find the latest unmarked var with the desired name
        if(lookup(current, lexlevel) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(EXIT_FAILURE);
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
        EXPRESSION(lexlevel);

        if(TOKEN != rparentsym)
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
