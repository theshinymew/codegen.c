#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lex.h"

int i = 0;

lexeme *list;

void PROGRAM()
{
    BLOCK();
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
    if(list[i].token == constsym)
    {
        do
        {
            if(list[i].token != identsym)
            {
                // ERROR
            }

            // save ident name

            if( 0 /* ident name is already in symbol table */)
            {
                // ERROR
            }
            i++;

            if(list[i].token != eqsym)
            {
                // ERROR
            }
            i++;

            if(list[i].token != numbersym)
            {
                // ERROR
            }

            // Add to symbol table.
            i++;
        } while (list[i].token == commasym);
        
        if(list[i].token != semicolonsym)
        {
            // ERROR
        }
        i++;
    }
}

// TODO: complete
void VAR_DECLARATION()
{
    int numVars;
    if(list[i].token == varsym)
    {
        numVars = 0;
        do
        {
            numVars++;
            i++;

            if(list[i].error > 0)
            {
                // ERROR
            }

            if(  0/* ident is already in symbol table */ )
            {
                // ERROR
            }

            // Add to symbol table
            i++;
        } while(list[i].token == commasym);

        if(list[i].token != semicolonsym)
        {
            // ERROR
        }
        i++;
    }
}

void STATEMENT()
{

}

void CONDITION()
{

}

void EXPRESSION()
{

}

void TERM()
{

}

void FACTOR()
{

}

symbol* parser(lexeme *lexList, int flag)
{
    // call program here
    //program will call the rest until it's done
    list = lexList;

    PROGRAM();

    symbol* table;
    return table;
}
