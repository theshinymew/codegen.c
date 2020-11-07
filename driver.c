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
        printf("ERROR: file name not given\n");
        return 1;
    }
    
    int lflag = 0, aflag = 0, vflag = 0;

    if(argc == 5)
    {
        lflag = 1;
        aflag = 1;
        vflag = 1;
    }
    else
    {
        for(int i = 2; i < argc; i++)
        {
            if(argv[i][1] == 'l')
                lflag = 1;
            else if(argv[i][1] == 'a')
                aflag = 1;
            else if(argv[i][1] == 'v')
                vflag = 1;
        }
    }    

    FILE *fp;
    if((fp = fopen(argv[1], "r")) == NULL)
    {
        printf("ERROR: input file could not be read\n");
        return 1;
    }

    //TODO: read input file and print to the screen

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
