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
        //printf("%s %d\n", l.name, l.token);

        // const declaration
        if (l.token == constsym)
        {
            printf("That's a constant!\n");
        }

        // var declaration
        if (l.token == varsym)
        {
            printf("That's a var!\n");
        }

        // statement

        // condition

        // expression

        // term

        // factor


    }
    return table;
}
