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
int tx; // symbol table current
int cx = 0; // code current
int tempcx;
char *name;

int regtoendupin = 0;

void CPROGRAM()
{
    // emit JMP for main
    emit(7, 0, 0, 1);

    CBLOCK();

    // emit HALT
    emit(9, 0, 0, 3);
}

void CBLOCK()
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

void CSTATEMENT()
{
    if(TOKEN == identsym)
    {
        name = list[current].name;
        // save symbol table current of identifier
        int current = lookup(name);
        current += 2;
        CEXPRESSION(0);

        //emit STO
        emit(4, 0, 0, table[current].addr);
    }

    if(TOKEN == beginsym)
    {
        current++;
        CSTATEMENT();

        while(TOKEN == semicolonsym)
        {
            current++;
            CSTATEMENT();
        }

        current++;
    }

    if(TOKEN == ifsym)
    {
        current++;
        CCONDITION();

        // store current code current to fix JPC later
        int jpccx = cx;
        // emit JPC
        emit(8, 0, 0, 0);

        current++;
        CSTATEMENT();

        // fix JPC by setting M to current current
        code[jpccx].m = cx;
    }

    if(TOKEN == whilesym)
    {
        current++;

        // store current code current for CCONDITION
        int condcx = cx;

        CCONDITION();

        current++;

        // store current code current for jump
        int jmpcx = cx;
        // emit JPC
        emit(8, 0, 0, 0);
        CSTATEMENT();

        // emit JMP
        emit(7, 0, 0, condcx);

        // fix JPC by setting M to current current
        code[jmpcx].m = cx;
    }

    if(TOKEN == readsym)
    {
        current++;

        // TODO: Save the symbol table current

        current++;
        emit (9, 0, 0, 2);
		emit (4, 0, 0, table[current].addr);
    }

    if(TOKEN == writesym)
    {
        current++;

        // TODO: Save the symbol table current
        // TODO: why are these CCONDITIONs different?

        if(0 /* it's a var */)
        {
            emit (3, 0, 0, table[current].addr);
			emit (9, 0, 0, 1);
        }

        if(0 /* it's a var */)
        {
            // TODO: what does this line mean
            //emit (1, 0, 0, M comes from the symbol table (val));
			emit (9, 0, 0, 1);
        }
        current++;
    }
}

void CCONDITION()
{
    if(TOKEN == oddsym)
    {
        current++;
        CEXPRESSION(0);
        emit (15, 0, 0, 0);
    }

    else
    {
        CEXPRESSION(0);
        if(TOKEN == eqsym)
        {
            current++;
            CEXPRESSION(1);
            emit (17, 0, 0, 1);
        }

        if(TOKEN == neqsym) // <>?
        {
            current++;
            CEXPRESSION(1);
            emit (18, 0, 0, 1);
        }

        if(TOKEN == lessym)
        {
            current++;
            CEXPRESSION(1);
            emit (19, 0, 0, 1);
        }

        if(TOKEN == leqsym)
        {
            current++;
            CEXPRESSION(1);
            emit (20, 0, 0, 1);
        }

        if(TOKEN == gtrsym)
        {
            current++;
            CEXPRESSION(1);
            emit (21, 0, 0, 1);
        }

        if(TOKEN == geqsym)
        {
            current++;
            CEXPRESSION(1);
            emit (22, 0, 0, 1);
        }
    }
    return;
}

void CEXPRESSION(int r)
{
    if(TOKEN == plussym)
    {
        current++;
    }

    if(TOKEN == minussym)
    {
        current++;
        CTERM(regtoendupin);
        emit(10, regtoendupin, 0, 0);

        while(TOKEN == plussym || TOKEN == minussym)
        {
            if(TOKEN == plussym)
            {
                current++;
                CTERM(regtoendupin + 1);
                emit(11, regtoendupin, regtoendupin, regtoendupin + 1);
            }

            if(TOKEN == minussym)
            {
                current++;
                CTERM(regtoendupin + 1);
                emit(12, regtoendupin, regtoendupin, regtoendupin + 1);
            }
        }
        return;
    }

    CTERM(regtoendupin);

    while(TOKEN == plussym || TOKEN == minussym)
    {
        if(TOKEN == plussym)
        {
            current++;
            CTERM(regtoendupin + 1);
            emit(11, regtoendupin, regtoendupin, regtoendupin + 1);
        }

        if(TOKEN == minussym)
        {
            current++;
            CTERM(regtoendupin + 1);
            emit(12, regtoendupin, regtoendupin, regtoendupin + 1);
        }
    }

    return;
}

void CTERM(int r)
{
    CFACTOR(regtoendupin);

    while(TOKEN == multsym || TOKEN == slashsym)
    {
        if(TOKEN == multsym)
        {
            current++;
			CFACTOR(regtoendupin + 1);
			emit(13, regtoendupin, regtoendupin, regtoendupin + 1);
        }

        if(TOKEN == slashsym)
        {
            current++;
			CFACTOR(regtoendupin + 1);
			emit(14, regtoendupin, regtoendupin, regtoendupin + 1);
        }
    }
    return;
}

void CFACTOR(int r)
{
    if(TOKEN == identsym)
    {
        // Save the symbol table current

        if(TOKEN == constsym)
        {
            emit(1, regtoendupin, 0, table[current].val);
        }

        if(TOKEN == varsym)
        {
            emit(3, regtoendupin, 0, table[current].addr);
        }
        current++;
    }

    else if(TOKEN == numbersym)
    {
        emit(1, regtoendupin, 0, table[current].val);
        current++;
    }

    else
    {
        current++;
		CEXPRESSION(regtoendupin);
		current++;
    }
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

    CPROGRAM();

    return code;

    // TODO: finish codegen
    // TODO: ctrl + f "regtoendupin" and add whatever that is
    /* NOTES: pseudocode is a mess
              when she says token + [something] that means advance current by that amount
              saving the code currentes is a pain you have to understand how the instruction works
              if you don't that's ok just do the other instructions
              cx is the code current, the current where the current CSTATEMENT is being saved
              check our hw1 vm.c to double check
              uhhhh anything else just ask me
    */

}
