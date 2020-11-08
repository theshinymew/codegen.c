// Alejandro Herrada
// Skyler Bolton

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

    char *inputFile = malloc(500 * sizeof(char));
    char c = fgetc(fp);
    int i = 0;

    while (1)
    {
    	inputFile[i++] = c;
    	c = fgetc(fp);
    	if (c == EOF)
    		break;
    }
    inputFile[i] = '\0';

    printf("%s\n", inputFile);

    lexeme *list = lexer(inputFile, lflag);
    symbol *table = parser(list);
    printf("No errors, program is syntactically correct\n\n");
    instruction *code = codegen(table, list, aflag);
    vm(code, vflag);
    return 0;
}
