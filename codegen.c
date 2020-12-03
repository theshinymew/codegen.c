// Alejandro Herrada
// Skyler Bolton

#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"

lexeme *list;
symbol *table;
instruction *code;

// Global variable to keep track of current token
int current;
int tx; // symbol table current
int cx = 0; // code current
char *name;

void CPROGRAM()
{
    int lexlevel = 0;
    // emit JMP for main
    emit(JMP, 0, 0, 1);

    CBLOCK(lexlevel);

    // emit HALT
    emit(SYS, 0, 0, 3);
}

void CBLOCK(int lexlevel)
{
    int numvars = 0;
    if(list[current].token == constsym)
    {
        do
        {
            current += 4;
        }
        while(list[current].token == commasym);

        current++;
    }
    if (list[current].token == varsym)
    {
        do
        {
            numvars++;
            current += 2;
        }
        while(list[current].token == commasym);

        current++;
    }

    // emit INC
    emit(INC, 0, 0, 3 + numvars);
    CSTATEMENT(lexlevel);
}

void CSTATEMENT(int lexlevel)
{
    if(list[current].token == identsym)
    {
        name = list[current].name;
        // save symbol table current of identifier
        int temp = lookup(current, lexlevel);
        current += 2;
        CEXPRESSION(0, lexlevel);

        //emit STO
        emit(STO, 0, 0, table[temp].addr);
    }

    if(list[current].token == beginsym)
    {
        current++;
        CSTATEMENT(lexlevel);

        while(list[current].token == semicolonsym)
        {
            current++;
            CSTATEMENT(lexlevel);
        }

        current++;
    }

    if(list[current].token == ifsym)
    {
        current++;
        CCONDITION(lexlevel);

        // store current code current to fix JPC later
        int jpccx = cx;
        // emit JPC
        emit(JPC, 0, 0, 0);

        current++;
        CSTATEMENT(lexlevel);

        // fix JPC by setting M to current current
        code[jpccx].m = cx;
    }

    if(list[current].token == whilesym)
    {
        current++;

        // store current code index current for condition
        int condcx = cx;

        CCONDITION(lexlevel);

        current++;

        // store current code index for jmp
        int jmpcx = cx;
        // emit JPC
        emit(JPC, 0, 0, 0);
        CSTATEMENT(lexlevel);

        // emit JMP
        emit(JMP, 0, 0, condcx);

        // fix JPC by setting M to current current
        code[jmpcx].m = cx;
    }

    if(list[current].token == readsym)
    {
        current++;

        name = list[current].name;
        // save symbol table current of identifier
        int temp = lookup(current, lexlevel);

        current++;

        // emit READ
        emit(SYS, 0, 0, 2);
        // emit STO
        emit(STO, 0, 0, table[temp].addr);
    }

    if(list[current].token == writesym)
    {
        current++;

        name = list[current].name;
        // save symbol table current of identifier
        int temp = lookup(current, lexlevel);

        // if it's a var
        if(table[temp].kind == 2)
        {
            // emit LOD
            emit(LOD, 0, 0, table[temp].addr);
            // emit WRITE
            emit(SYS, 0, 0, 1);
        }
        // if it's a const
        if(table[temp].kind == 1)
        {
            // emit LIT
            emit(LIT, 0, 0, table[temp].addr);
            // emit WRITE
            emit(SYS, 0, 0, 1);
        }
        current++;
    }
}

void CCONDITION(int lexlevel)
{
    if(list[current].token == oddsym)
    {
        current++;
        CEXPRESSION(0, lexlevel);
        emit(ODD, 0, 0, 0);
    }
    else
    {
        CEXPRESSION(0, lexlevel);
        if(list[current].token == eqsym)
        {
            current++;
            CEXPRESSION(1, lexlevel);
            emit(EQL, 0, 0, 1);
        }
        if(list[current].token == neqsym)
        {
            current++;
            CEXPRESSION(1, lexlevel);
            emit(NEQ, 0, 0, 1);
        }
        if(list[current].token == lessym)
        {
            current++;
            CEXPRESSION(1, lexlevel);
            emit(LSS, 0, 0, 1);
        }
        if(list[current].token == leqsym)
        {
            current++;
            CEXPRESSION(1, lexlevel);
            emit(LEQ, 0, 0, 1);
        }

        if(list[current].token == gtrsym)
        {
            current++;
            CEXPRESSION(1, lexlevel);
            emit(GTR, 0, 0, 1);
        }

        if(list[current].token == geqsym)
        {
            current++;
            CEXPRESSION(1, lexlevel);
            emit(GEQ, 0, 0, 1);
        }
    }
}

void CEXPRESSION(int r, int lexlevel)
{
    if(list[current].token == plussym)
    {
        current++;
    }

    if(list[current].token == minussym)
    {
        current++;
        CTERM(r, lexlevel);
        emit(NEG, r, 0, 0);

        while(list[current].token == plussym || list[current].token == minussym)
        {
            if(list[current].token == plussym)
            {
                current++;
                CTERM(r + 1, lexlevel);
                emit(ADD, r, r, r + 1);
            }

            if(list[current].token == minussym)
            {
                current++;
                CTERM(r + 1, lexlevel);
                emit(SUB, r, r, r + 1);
            }
        }
        return;
    }

    CTERM(r, lexlevel);

    while(list[current].token == plussym || list[current].token == minussym)
    {
        if(list[current].token == plussym)
        {
            current++;
            CTERM(r + 1, lexlevel);
            emit(ADD, r, r, r + 1);
        }

        if(list[current].token == minussym)
        {
            current++;
            CTERM(r + 1, lexlevel);
            emit(SUB, r, r, r + 1);
        }
    }
}

void CTERM(int r, int lexlevel)
{
    CFACTOR(r,lexlevel);

    while(list[current].token == multsym || list[current].token == slashsym)
    {
        if(list[current].token == multsym)
        {
            current++;
			CFACTOR(r + 1, lexlevel);
			emit(MUL, r, r, r + 1);
        }

        if(list[current].token == slashsym)
        {
            current++;
			CFACTOR(r + 1, lexlevel);
			emit(DIV, r, r, r + 1);
        }
    }
    return;
}

void CFACTOR(int r, int lexlevel)
{
    if(list[current].token == identsym)
    {
        name = list[current].name;
        // save symbol table current of identifier
        int temp = lookup(current, lexlevel);

        if(table[temp].kind == 1)
        {
            emit(LIT, r, 0, table[temp].val);
        }
        if(table[temp].kind == 2)
        {
            emit(LOD, r, 0, table[temp].addr);
        }
        current++;
    }
    else if(list[current].token == numbersym)
    {
        emit(LIT, r, 0, list[current].value);
        current++;
    }
    else
    {
        current++;
		CEXPRESSION(r, lexlevel);
		current++;
    }
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

instruction* codegen(symbol *symbol_table, lexeme *lexeme_list, int flag)
{
    table = symbol_table;
    list = lexeme_list;
    code = malloc(sizeof(instruction) * 500);

    current = 0;
    CPROGRAM();

    if(flag)
    {
        printf("Line OP  R   L   M\n");
	    for(int i = 0; i < cx; i++)
	    {
            // Print line and opcode.
            printf("%-4d %s ", i, opcodes[code[i].op]);

            // Print line of instructions.
            printf("%-3d %-3d %-3d\n", code[i].r, code[i].l, code[i].m);
	    }
	    printf("\n");
    }

    return code;
}
