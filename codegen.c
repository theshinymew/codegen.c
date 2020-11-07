#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"

#define TOKEN list[current].token // this is a pain to type

lexeme *list;
symbol *table;
instruction *code;

// Global variable to keep track of current token
int current = 0;
int tx; // symbol table index
int cx = 0; // code index
int tempcx;
char *name;

void PROGRAM()
{
    // emit JMP for main
    emit(7, 0, 0, 1);
    
    BLOCK();

    // emit HALT
    emit(9, 0, 0, 3);
}

void BLOCK()
{
    int numvars = 0;
    if(TOKEN == constsym)
    {
        do
        {
            current += 4;
        } 
        while(TOKEN == commasym);
        
        current++;
    }
    if (TOKEN == varsym)
    {
        do
        {
            numvars++;
            current += 2;
        } 
        while(TOKEN == commasym);
        
        current++;        
    }

    // emit INC
    emit(6, 0, 0, 3 + numvars);
}

void STATEMENT()
{
    if(TOKEN == identsym)
    {
        name = list[current].name;
        // save symbol table index of identifier
        int index = lookup(name);
        current += 2;
        EXPRESSION(0);

        //emit STO
        emit(4, 0, 0, table[index].addr);
    }

    if(TOKEN == beginsym)
    {
        current++;
        STATEMENT();

        while(TOKEN == semicolonsym)
        {
            current++;
            STATEMENT();
        }

        current++;        
    }

    if(TOKEN == ifsym)
    {
        current++;
        CONDITION();
        
        // store current code index to fix JPC later
        int jpccx = cx;
        // emit JPC
        emit(8, 0, 0, 0);
        
        current++;
        STATEMENT();

        // fix JPC by setting M to current index
        code[jpccx].m = cx;
    }

    if(TOKEN == whilesym)
    {
        current++;
        
        // store current code index for condition
        int condcx = cx;

        CONDITION();

        current++;

        // store current code index for jump
        int jmpcx = cx;
        // emit JPC
        emit(8, 0, 0, 0);
        STATEMENT();

        // emit JMP
        emit(7, 0, 0, condcx);

        // fix JPC by setting M to current index
        code[jmpcx].m = cx;
    }
    
    // TODO: read and write for STATEMENT
}

void CONDITION()
{
    return;
}

void EXPRESSION(int r)
{
    return;
}

void TERM()
{
    return;
}

void FACTOR()
{
    return;
}

void emit(int op, int r, int l, int m)
{
    if(cx > MAX_TABLE_SIZE)
    {
        printf("ERROR: too much code\n");
        exit(1);
    }
    else
    {
        code[cx].op = op;
        code[cx].r = r;
        code[cx].l = l;
        code[cx].m = m;
        cx++;
    }
    
}

instruction* codegen(symbol *symbol_table, lexeme *lexeme_list)
{
    table = symbol_table;
    list = lexeme_list;
    code = malloc(sizeof(instruction) * 500);
    
    PROGRAM();
    
    return code;

    // TODO: finish codegen
    /* NOTES: pseudocode is a mess
              when she says token + [something] that means advance current by that amount
              saving the code indexes is a pain you have to understand how the instruction works
              if you don't that's ok just do the other instructions
              cx is the code index, the index where the current statement is being saved 
              check our hw1 vm.c to double check
              uhhhh anything else just ask me
    */

}