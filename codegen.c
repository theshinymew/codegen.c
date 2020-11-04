#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"

instruction* generate_code(symbol *table, lexeme *list)
{
    instruction *code = malloc(sizeof(instruction) * 500);
    printf("code has been generated\n"); // debug message? taken from noelle's
    return code;
}