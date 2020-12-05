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

int current; // token current
int cx; // code current
char *name;

// find closest unmarked going backwards
int closestlookup(char *name, int kind, int level)
{
    for(int i = symcount; i >= 0; i--)
    {
        if(!strcmp(table[i].name, name) && table[i].kind == kind && table[i].level <= level && table[i].mark == 0)
            return i;
    }
}

// find marked ident to unmark
int markedlookup(char *name, int kind, int level)
{
    for(int i = 0; i < symcount; i++)
    {
        if(!strcmp(name, table[i].name) && table[i].level == level && table[i].mark == 1)
            return i;
    }
}

// find closest unmarked going backwards and not considering kind
int closestlookupnokind(char *name, int level)
{
    for(int i = symcount; i >= 0; i--)
    {
        if(!strcmp(table[i].name, name) && table[i].level <= level && table[i].mark == 0 
            && (table[i].kind == CONST || table[i].kind == VAR))
            return i;
    }
}

void CPROGRAM()
{
    int numproc = 1;
    for(int i = 0; i < symcount; i++)
    {
        if(table[i].kind == PROC)
        {
            table[i].val = numproc;
            numproc++;
            // emit JMP for each procedure
            emit(JMP, 0, 0, 0);
        }
    }

    CBLOCK(0, 0);

    // fix JMP codes
    int proc = 0;
    for(int i = 0; code[i].op == JMP; i++)
    {
        // find next procedure in symbol table
        while((table[proc].kind != PROC) && (proc < symcount))
        {
            proc++;
        }

        code[i].m = table[proc].addr;
        proc++;        
    }

    // fix CAL codes
    for(int i = 0; i < cx; i++)
    {
        if(code[i].op == CAL)
        {
            // fix M in CAL  
            for(int j = 0; j < symcount; j++)
            {
                if((table[j].kind == PROC) && (table[j].val == code[i].m))
                {
                    code[i].m = table[j].addr;
                    break;
                }
            }
        }
    }

    // emit HALT
    emit(SYS, 0, 0, 3);
}

void CBLOCK(int lexlevel, int cproc)
{
    int numvars = 0;
    int numsymbols = 0;
    if(TOKEN == constsym)
    {
        do
        {
            current++;
            numsymbols++;
            // unmark const in symbol table
            int temp = markedlookup(TNAME, CONST, lexlevel);
            table[temp].mark = 0;
        } while(TOKEN == commasym);

        current++;
    }
    if(TOKEN == varsym)
    {
        do
        {
            current++;
            numvars++;
            numsymbols++;
            // unmark var in symbol table
            int temp = markedlookup(TNAME, VAR, lexlevel);
            table[temp].mark = 0;
        } while(TOKEN == commasym);

        current++;
    }
    if(TOKEN == procsym)
    {
        do
        {
            current++;
            numsymbols++;
            // unmark proc in symbol table
            int temp = markedlookup(TNAME, PROC, lexlevel);
            table[temp].mark = 0;
            current += 2;
            CBLOCK(lexlevel + 1, temp);
            //emit RTN
            emit(RTN, 0, 0, 0);
            current++;
        } while (TOKEN == procsym);        
    }

    // update symbol table for this procedure
    table[cproc].addr = cx;
    // emit INC
    emit(INC, 0, 0, 3 + numvars);
    CSTATEMENT(lexlevel);
    // mark the last numsymbols number of marked symbols
    // TODO
}

void CSTATEMENT(int lexlevel)
{
    if(TOKEN == identsym)
    {
        int temp = closestlookup(TNAME, VAR, lexlevel);
        current += 2;
        CEXPRESSION(0, lexlevel);
        //emit STO
        emit(STO, lexlevel - table[temp].level, table[temp].addr);
    }

    if(TOKEN == callsym)
    {
        current++;
        int temp = closestlookup(TNAME, PROC, lexlevel);
        // emit CAL
        emit(CAL, 0, lexlevel - table[temp].level, table[temp].addr);
        current++;
    }

    if(TOKEN == beginsym)
    {
        current++;
        CSTATEMENT(lexlevel);
        while(TOKEN == semicolonsym)
        {
            current++;
            CSTATEMENT(lexlevel);
        }
        current++;
    }

    if(TOKEN == ifsym)
    {
        current++;
        CCONDITION(lexlevel);
        // store current code current to fix JPC later
        int jpccx = cx;
        // emit JPC
        emit(JPC, 0, 0, 0);
        current++;
        CSTATEMENT(lexlevel);

        if(TOKEN == elsesym)
        {
            current++;
            // store the current index for JMP
            int jmpcx = cx;
            // emit JMP
            emit(JMP, 0, 0, 0);
            // fix JPC from earlier
            code[jpccx].m = cx;
            CSTATEMENT(lexlevel);
            // fix JMP from earlier
            code[jmpcx].m = cx;
        }
        else
        {
            // fix JPC from earlier
            code[jpccx].m = cx;
        }
    }

    if(TOKEN == whilesym)
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
        // fix JPC from earlier
        code[jmpcx].m = cx;
    }

    if(TOKEN == readsym)
    {
        current++;
        int temp = closestlookup(TNAME, VAR, lexlevel);
        current++;
        // emit READ
        emit(SYS, 0, 0, 2);
        // emit STO
        emit(STO, 0, lexlevel - table[temp].level, table[temp].addr);
    }

    if(TOKEN == writesym)
    {
        current++;
        CEXPRESSION(0, lexlevel);
        // emit WRITE
        emit(SYS, 0, 0, 1);
    }
}

void CCONDITION(int lexlevel)
{
    if(TOKEN == oddsym)
    {
        current++;
        CEXPRESSION(0, lexlevel);
        emit(ODD, 0, 0, 0);
    }
    else
    {
        CEXPRESSION(0, lexlevel);
        if(TOKEN == eqsym)
        {
            current++;
            CEXPRESSION(1, lexlevel);
            emit(EQL, 0, 0, 1);
        }
        if(TOKEN == neqsym)
        {
            current++;
            CEXPRESSION(1, lexlevel);
            emit(NEQ, 0, 0, 1);
        }
        if(TOKEN == lessym)
        {
            current++;
            CEXPRESSION(1, lexlevel);
            emit(LSS, 0, 0, 1);
        }
        if(TOKEN == leqsym)
        {
            current++;
            CEXPRESSION(1, lexlevel);
            emit(LEQ, 0, 0, 1);
        }

        if(TOKEN == gtrsym)
        {
            current++;
            CEXPRESSION(1, lexlevel);
            emit(GTR, 0, 0, 1);
        }

        if(TOKEN == geqsym)
        {
            current++;
            CEXPRESSION(1, lexlevel);
            emit(GEQ, 0, 0, 1);
        }
    }
}

void CEXPRESSION(int r, int lexlevel)
{
    if(TOKEN == plussym)
    {
        current++;
    }

    if(TOKEN == minussym)
    {
        current++;
        CTERM(r, lexlevel);
        emit(NEG, r, 0, 0);
        while(TOKEN == plussym || TOKEN == minussym)
        {
            if(TOKEN == plussym)
            {
                current++;
                CTERM(r + 1, lexlevel);
                emit(ADD, r, r, r + 1);
            }
            if(TOKEN == minussym)
            {
                current++;
                CTERM(r + 1, lexlevel);
                emit(SUB, r, r, r + 1);
            }
        }
        return;
    }
    CTERM(r, lexlevel);
    while(TOKEN == plussym || TOKEN == minussym)
    {
        if(TOKEN == plussym)
        {
            current++;
            CTERM(r + 1, lexlevel);
            emit(ADD, r, r, r + 1);
        }
        if(TOKEN == minussym)
        {
            current++;
            CTERM(r + 1, lexlevel);
            emit(SUB, r, r, r + 1);
        }
    }
}

void CTERM(int r, int lexlevel)
{
    CFACTOR(r, lexlevel);
    while(TOKEN == multsym || TOKEN == slashsym)
    {
        if(TOKEN == multsym)
        {
            current++;
			CFACTOR(r + 1, lexlevel);
			emit(MUL, r, r, r + 1);
        }
        if(TOKEN == slashsym)
        {
            current++;
			CFACTOR(r + 1, lexlevel);
			emit(DIV, r, r, r + 1);
        }
    }
}

void CFACTOR(int r, int lexlevel)
{
    if(TOKEN == identsym)
    {
        int temp = closestlookupnokind(TNAME, lexlevel);        
        if(table[temp].kind == CONST)
        {
            emit(LIT, r, 0, table[temp].val);
        }
        if(table[temp].kind == VAR)
        {
            emit(LOD, r, 0, table[temp].addr);
        }
        current++;
    }
    else if(TOKEN == numbersym)
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
    cx = 0;
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