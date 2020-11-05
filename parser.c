#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lex.h"

symbol* parser(lexeme *list, int flag, int j)
{
    int i;
    lexeme l;
    symbol table[500];

    printf("\nj = %d\n", j);
    for(i = 0; i < j; i++)
    {
        l = list[i];

        printf("\ni = %d\n", i);

        /*
        Check lecture 05a.
        For constants, you must store kind = 1, name and value.
        For variables, you must store kind = 2, name, L and M.
        For procedures, you must store kind = 3, name, L and M.
        */

        // table[i].kind = ; // const = 1, var = 2, proc = 3

        printf("Getting table[i].name.\n");
        strcpy(table[i].name, l.name);

        printf("Getting table[i].value.\n");
    	table[i].val = l.value;

        printf("Getting table[i].level.\n");
    	table[i].level = 0;

    	//table[i].addr; // M address
    	//table[i].mark; // 0 = in use for code generation, 1 = unavailable
    }

    return table;
}
