#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"
#include "vm.h"

int main(int argc, char **argv)
{
    int j = 0;

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

    //TODO: set flags according to inputs

    //TODO: modify lex.c, vm.c: change main to method signature
    //TODO: move typedef declaratiosn to .h file?
    //TODO: remove file i/o from other vm.c
    //TODO: wrap print statements around flag condition: if(flag){print...}

    lexeme *list = lexer(fp, lflag, &j);
    symbol *table = parser(list, aflag, j);
    // does codegen get aflag too?
    instruction *code = generate_code(table, list);
    virtualMachine(code, vflag);

    return 0;
}
