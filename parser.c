#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lex.h"

int current, symcount, varcount = 0;

lexeme *list;
symbol *table;

void insert(int kind, char* name, int value, int level, int address, int mark)
{
    table[symcount].kind = kind;
    strcpy(table[symcount].name, name);
    table[symcount].val = value;
    table[symcount].level = level;
    table[symcount].addr = address;
    table[symcount].mark = mark;
}

int lookup(char* name)
{
    for(int i = 0; i < symcount; i++)
    {
        if(!strcasecmp(name, table[i].name))
            return 1;
    }
    return 0;
}

void PROGRAM()
{
    BLOCK();
    if(list[current].token != periodsym)
        printf("ERROR: Expected period terminator\n");
}

void BLOCK()
{
    CONST_DECLARATION();
    VAR_DECLARATION();
    STATEMENT();
}

// TODO: complete
void CONST_DECLARATION()
{
    printf("inside const_declaration, current = %d\n", current);
    if(list[current].token == constsym)
    {
        do
        {
            current++;
            if(list[current].token != identsym)
            {
                // ERROR
            }

            // save ident name
            char *name = list[current].name;

            if( 0 /* ident name is already in symbol table */)
            {
                // ERROR
            }
            current++;

            if(list[current].token != eqsym)
            {
                // ERROR
            }
            current++;

            if(list[current].token != numbersym)
            {
                // ERROR
            }


            // Add to symbol table.
            insert(1, name, list[current].value, 0, 0, 0);
            symcount++;

            current++;
        } while(list[current].token == commasym);

        if(list[current].token != semicolonsym)
        {
            // ERROR
        }
        current++;
    }
}

// TODO: complete
void VAR_DECLARATION()
{
    if(list[current].token == varsym)
    {
        do
        {
            symcount++;
            varcount++;
            current++;

            if(list[current].error > 0)
            {
                // ERROR
            }

            if(  0/* ident is already in symbol table */ )
            {
                // ERROR
            }

            // Add to symbol table
            insert(2, list[current].name, 0, 0, varcount + 2, 0); // wrong?
            symcount++;
            varcount++;

            current++;
        } while(list[current].token == commasym);

        if(list[current].token != semicolonsym)
        {
            // ERROR
        }
        current++;
    }
}

void STATEMENT()
{
    if(list[current].token == identsym)
    {
        if(0 /* not already in symbol table */ )
        {
            // ERROR
        }
        if(0 /* ident != var */ )
        {
            // ERROR
        }

        current++;

        if(list[current].token != becomessym)
        {
            // ERROR
        }

        current++;
        EXPRESSION();
    }
    else if(list[current].token == beginsym)
    {

    }
    else if(list[current].token == ifsym)
    {

    }
    else if(list[current].token == whilesym)
    {

    }
    else if(list[current].token == readsym)
    {

    }
    else if(list[current].token == writesym)
    {

    }
}

void CONDITION()
{
    if(list[current].token == oddsym)
    {

    }
    else
    {

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

    }
    else if(list[current].token == numbersym)
    {

    }
    else if(0 /* something */)
    {

    }
    else
    {
        // ERROR
    }
}

symbol* parser(lexeme *lexList, int flag)
{
    // call program here
    //program will call the rest until it's done
    list = lexList;
    table = malloc(sizeof(symbol) * MAX_TABLE_SIZE);

    printf("in parser(), calling program()\n");
    printf("passed: %p \t global: %p\n", lexList, list);

    PROGRAM();
    return table;
}
