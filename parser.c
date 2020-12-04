// Alejandro Herrada
// Skyler Bolton

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lex.h"

lexeme *list;
symbol *table;
char *name;

// Global variables to keep track of current token, symbol table capacity, and variable counter
int current = 0;
int symcount = 0;

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

int reverselookup(char *name, int kind)
{
    for(int i = symcount - 1; i >= 0; i--)
    {
        if(!strcmp(table[i].name, name) && table[i].kind == kind && table[i].mark == 0)
            return i;
    }
    return -1;
}

void markbackwards(int n)
{
    int s = symcount - 1;

    while(n > 0 && s >= 0)
    {
        if(!table[s].mark)
        {
            table[s].mark = 1;
            n--;
        }
        s--;
    }
    
}

void printtable()
{
    for(int i = 0; i < symcount; i++)
    {
        printf("Symbol Table:\nkind: %d name: %11s value: %6d level: %3d addr: %3d mark: %d\n", table[i].kind, 
        table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);
    }
}

void PROGRAM()
{
    insert(PROC, "main", 0, 0, 0, 0);
    
    printtable();
    
    BLOCK(0);

    if(TOKEN != periodsym)
    {
        printf("ERROR: period expected\n");
        exit(EXIT_FAILURE);
    }
}

void BLOCK(int lexlevel)
{
    int numsybols = 0;
    numsybols += CONST_DECLARATION(lexlevel);
    numsybols += VAR_DECLARATION(lexlevel);
    numsybols += PROC_DECLARATION(lexlevel); // +1 is inside procdec
    printf("in BLOCK before STATEMENT\n"); printtable();
    STATEMENT(lexlevel);
    printf("in BLOCK after STATEMENT\n"); printtable();
    // mark the last numsymbols number of unmarked symbols
    markbackwards(numsybols);
    printf("in BLOCK after mark backwards\n"); printtable();
}

int CONST_DECLARATION(int lexlevel)
{
    int numconsts = 0; 
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
            name = TNAME;
            int temp = lookup(name);
            if(temp != -1)
            {
                if(table[temp].mark == 0 && table[temp].level == lexlevel)
                {
                    printf("ERROR: identifier has already been declared\n");
                    exit(EXIT_FAILURE);
                }
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
            insert(CONST, name, list[current].value, 0, 0, 0);
            numconsts++;
            current++;
        } while(TOKEN == commasym);

        if(TOKEN != semicolonsym)
        {
            printf("ERROR: semicolon expected\n");
            exit(EXIT_FAILURE);
        }
        current++;
    }
    
    return numconsts;
}

int VAR_DECLARATION(int lexlevel)
{
    int numvars = 0;
    if(TOKEN == varsym)
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
            name = TNAME;
            int temp = lookup(name);
            if(temp != -1)
            {
                if(table[temp].mark == 0 && table[temp].level == lexlevel)
                {
                    printf("ERROR: identifier has already been declared\n");
                    exit(EXIT_FAILURE);
                }
            }

            // Add to symbol table.
            insert(VAR, name, 0, 0, numvars + 2, 0);
            numvars++;
            current++;
        } while(TOKEN == commasym);

        if(TOKEN != semicolonsym)
        {
            printf("ERROR: semicolon expected\n");
            exit(EXIT_FAILURE);
        }
        current++;
    }
    return numvars;
}

int PROC_DECLARATION(int lexlevel)
{
    int numprocs = 0;
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

            // save ident name
            name = TNAME;
            int temp = lookup(name);
            if(temp != -1)
            {
                if(table[temp].mark == 0 && table[temp].level == lexlevel)
                {
                    printf("ERROR: identifier has already been declared\n");
                    exit(EXIT_FAILURE);
                }
            }

            // Add to the symbol table
            insert(PROC, name, 0, lexlevel, 0, 0);
            numprocs++;
            current++;

            if(TOKEN != semicolonsym)
            {
                printf("ERROR: semicolon expected\n");
                exit(EXIT_FAILURE);
            }
            current++;

            BLOCK(lexlevel + 1);

            if(TOKEN != semicolonsym)
            {
                printf("ERROR: semicolon expected\n");
                exit(EXIT_FAILURE);
            }
            current++;

        } while(TOKEN == procsym);
    }
    return numprocs;
}

void STATEMENT(int lexlevel)
{
    if(TOKEN == identsym)
    {
        if(backwardslookup(TNAME, VAR) == -1)
        {
            printf("ERROR: undeclared identifier\n");
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
        if(TOKEN != identsym)
        {
            printf("ERROR: call must be followed by identifier\n");
            exit(EXIT_FAILURE);
        }

        if(backwardslookup(TNAME, PROC) == -1)
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
        if(TOKEN == thensym)
        {
            current++;
            STATEMENT(lexlevel);
        }
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

        if(backwardslookup(TNAME, VAR) == -1)
        {
            printf("ERROR: undeclared identifier\n");
            exit(EXIT_FAILURE);
        }
        current++;
        return;
    }

    if(TOKEN == writesym)
    {
        current++;
        EXPRESSION(lexlevel);
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

void FACTOR(int lexlevel)
{
    if(TOKEN == identsym)
    {
        if(backwardslookup(TNAME, VAR) == -1)
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
