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

    for(i = 0; i < j; i++)
    {
        l = list[i];

        //table[i].kind = ;
        //strcpy(table[i].name, l.name);
    	//table[i].val = l.value;
    	//table[i].level = 0;
    	//table[i].addr;
    	//table[i].mark;
    }

    return table;
}
