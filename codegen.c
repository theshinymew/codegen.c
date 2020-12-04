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
    // emit JMP for every procedure
    int procCount = 1;
    for(int i = 0; i < symcount; i++)
    {
        if(table[i].kind == PROC)
        {
            table[i].val = procCount;
            procCount++;
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

        code[i].m = table[proc].val;
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
    int numsymbols = 0;
    int numvars = 0;
    if(TOKEN == constsym)
    {
        do
        {
            current++;
            numsymbols++;
            // unmark const in symbol table
            tx = lookup(current, lexlevel);
            table[tx].mark = 0;
            
            current += 3;
        } while(TOKEN == commasym);

        current++;
    }
    
    if(TOKEN == varsym)
    {
        do
        {
            current++;
            numvars++;
            numsymbols;
            // unmark var in symbol table
            tx = lookup(current, lexlevel);
            table[tx].mark = 0;
            current++;
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
            tx = lookup(current, lexlevel);
            table[tx].mark = 0;
            
            CBLOCK(lexlevel + 1, tx);

            // emit RTN
            emit(RTN, 0, 0, 0);

            current++;
        } while (TOKEN == procsym);
    }

    // update symbol table entry for this procedure
    table[cproc].addr = cx;
    emit(INC, 0, 0, 3 + numvars);
    CSTATEMENT(lexlevel);

    // emit INC
    emit(INC, 0, 0, 3 + numvars);
    CSTATEMENT(lexlevel);

    // mark the last numsymbols number of unmarked symbols
    while(numsymbols > 0)
    {
        if(!table[cproc].mark)
        {
            table[cproc].mark = 1;
            numsymbols--;
        }        
        cproc++;
    }
}

void CSTATEMENT(int lexlevel)
{
    if(TOKEN == identsym)
    {
        // save symbol table index of current identifier
        tx = lookup(current, lexlevel);
        current += 2;
        CEXPRESSION(0, lexlevel);

        //emit STO
        emit(STO, 0, lexlevel - table[tx].level, table[tx].addr);
    }

    if(TOKEN == callsym)
    {
        current++;
        // save symbol table index of current procedure
        tx = lookup(current, lexlevel);

        // emit CAL
        emit(CAL, 0, table[tx].level, table[tx].addr);
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
            // store current code index to fix this JMP too later
            int jmpcx = cx;
            // emit JMP
            emit(JMP, 0, 0, 0);
            // fix JPC by setting M to current code index
            code[jpccx].m = cx;

            CSTATEMENT(lexlevel);

            // fix JMP by setting M to current code index
            code[jmpcx].m = cx;
        }
        else
        {
            // fix JPC by setting M to current code index
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
        // store current code index for JMP
        int jmpcx = cx;
        // emit JPC
        emit(JPC, 0, 0, 0);

        CSTATEMENT(lexlevel);

        // emit JMP
        emit(JMP, 0, 0, condcx);
        // fix JPC by setting M to current current
        code[jmpcx].m = cx;
    }

    if(TOKEN == readsym)
    {
        current++;
        // save symbol table current of identifier
        tx = lookup(current, lexlevel);
        current++;
        // emit READ
        emit(SYS, 0, 0, 2);
        // emit STO
        emit(STO, 0, table[tx].level, table[tx].addr);
    }

    if(TOKEN == writesym)
    {
        current++;
        CEXPRESSION(lexlevel);
        //emite WRITE
        (SYS, 0, 0, 1);
        current++;
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
    CFACTOR(r,lexlevel);

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
    return;
}

void CFACTOR(int r, int lexlevel)
{
    if(TOKEN == identsym)
    {
        // save symbol table current of identifier
        tx = lookup(current, lexlevel);

        if(table[tx].kind == CONST)
        {
            emit(LIT, r, 0, table[tx].val);
        }
        if(table[tx].kind == VAR)
        {
            emit(LOD, r, 0, table[tx].addr);
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
