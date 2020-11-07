#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"
#include "vm.h"

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("error: file name not given\n");
        return 1;
    }
    int lflag = 0, aflag = 0, vflag = 0;

    FILE *fp;
    if((fp = fopen(argv[1], "r")) == NULL)
    {
        printf("error: input file could not be read\n");
        return 1;
    }
    
    for(int i = 2; argv[i] != NULL; i++)
    {
        if(strcmp(argv[i], "-l") == 0)
        {
            lflag = 1;
        }
        else if(strcmp(argv[i], "-v") == 0)
        {
            vflag = 1;
        }
        else if(strcmp(argv[i], "-a") == 0)
        {
            aflag = 1;
        }
    }

    //TODO: modify vm.c: change main to method signature
    //TODO: move typedef declaratiosn to .h file?
    //TODO: remove file i/o from other vm.c
    //TODO: wrap print statements around flag condition: if(flag){print...}

    printf("Lex analysis with flag lflag: %d\n\n", lflag);
    lexeme *list = lexer(fp, lflag);
    printf("Parsing with flag aflag: %d\n\n", aflag);
    printf("List is pointing to %p", list);
    symbol *table = parser(list, aflag);
    printf("Generating instructions\n\n");
    instruction *code = codegen(table, list);
    printf("Executing instructions with flag vflag: %d\n\n", vflag);
    vm(code, vflag);
    return 0;
}
