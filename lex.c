// Alejandro Herrada
// Skyler Bolton

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "lex.h"

token_type getAlphaTokenType(char *s)
{
	for(int i = 0; i < 14; i++)
	{
		if(!strcmp(s, reserved[i]))
			if(i == 0)
				return oddsym;
			else
				return i + 20;
	}

	return identsym;
}

lexeme* lexer(char *inputFile, int flag)
{
	// Initialize variables
	lexeme *lexeme_table = malloc(sizeof(lexeme) * MAX_TABLE_SIZE);
	lexeme l;
	char ch;
	char buffer[32];
	int i, j, k = 0;
	error_type error;

	// Initialize symbol hash table
	ssym['+'] = plussym;
	ssym['-'] = minussym;
	ssym['*'] = multsym;
	ssym['/'] = slashsym;
	ssym['('] = lparentsym;
	ssym[')'] = rparentsym;
	ssym['='] = eqsym;
	ssym[','] = commasym;
	ssym['.'] = periodsym;
	ssym['<'] = lessym;
	ssym['>'] = gtrsym;
	ssym[';'] = semicolonsym;
	ssym[':' * 2 + '='] = becomessym;
	ssym['<' * 2 + '>'] = neqsym;
	ssym['<' * 2 + '='] = leqsym;
	ssym['>' * 2 + '='] = geqsym;
	ssym['/' * 2 + '*'] = commentsym;
	ssym['*' * 2 + '/'] = endcommentsym;

	ch = inputFile[0];
	while(ch != '\0')
	{
		// Ignore whitespace
		if(isspace(ch) || iscntrl(ch))
		{
			ch = inputFile[++k];
		}

		// Reserved word or identifier
		else if(isalpha(ch))
		{
			buffer[0] = ch;
			i = 1;
			error = 0;
			// Continue adding to buffer
			while((ch = inputFile[++k]) != '\0' && isalnum(ch))
			{
				buffer[i] =  ch;
				i++;
			}

			// Add lexeme to table
			buffer[i] = '\0';
			lexeme_table[j].name = malloc(sizeof(char) * (i+1));
			strcpy(lexeme_table[j].name, buffer);
			lexeme_table[j].token = getAlphaTokenType(buffer);
			if(i > 12 && lexeme_table[j].token == identsym)
			{
				error = idtoolong;
				printf("ERROR: identifier is too long\n");
				exit(EXIT_FAILURE);
			}
			lexeme_table[j].error = error;

			j++;
		}
		// Number or invalid identifier
		else if(isdigit(ch))
		{
			buffer[0] = ch;
			i = 1;
			error = 0;
			// Continue adding to buffer
			while((ch = inputFile[++k]) != '\0' && isalnum(ch))
			{
				buffer[i] = ch;
				i++;

				if(isalpha(ch))
				{
					error = invalidid;
					printf("ERROR: invalid identifier\n");
					exit(EXIT_FAILURE);
				}
			}

			// Add lexeme to table
			if(error == 0)
			{
				if(atoi(buffer) > 99999)
				{
					error = numtoolong;
					printf("ERROR: number is too long\n");
					exit(EXIT_FAILURE);
				}
			}

			buffer[i] = '\0';
			if(error) // If it's an invalid identifier
			{
				lexeme_table[j].name = malloc(sizeof(char) * (i+1));
				strcpy(lexeme_table[j].name, buffer);
				lexeme_table[j].token = identsym;
				lexeme_table[j].error = error;
			}
			else // If it's an integer
			{
				lexeme_table[j].value = atoi(buffer);
				lexeme_table[j].token = numbersym;
				if(i > 6)
				{
					error = numtoolong;
					printf("ERROR: number is too long\n");
					exit(EXIT_FAILURE);
				}
				lexeme_table[j].error = error;
			}

			j++;
		}
		else
		{
			buffer[0] = ch;
			i = 1;
			error = 0;

			if(ch == '+' || ch == '-' || ch == '*' || ch == '=' || ch == '(' ||
				ch == ')' || ch == ',' || ch == ';' || ch == '.')
			{
				buffer[i] = '\0';

				// Add lexeme to table
				lexeme_table[j].name = malloc(sizeof(char) * (i+1));
				strcpy(lexeme_table[j].name, buffer);
				lexeme_table[j].token = ssym[ch];
				lexeme_table[j].error = error;

				j++;
			}
			else if(ch == '<')
			{
				ch = inputFile[++k];;
				if(ch == '>' || ch == '=') // "<>" | "<=" tokens
				{
					buffer[i] = ch;
					i++;

					buffer[i] = '\0';

					// Add lexeme to table
					lexeme_table[j].name = malloc(sizeof(char) * (i+1));
					strcpy(lexeme_table[j].name, buffer);
					lexeme_table[j].token = ssym[buffer[0] * 2 + buffer[1]];
					lexeme_table[j].error = error;

					j++;
				}
				else // "<" token
				{
					buffer[i] = '\0';

					// Add lexeme to table
					lexeme_table[j].name = malloc(sizeof(char) * (i+1));
					strcpy(lexeme_table[j].name, buffer);
					lexeme_table[j].token = lessym;
					lexeme_table[j].error = error;

					j++;
				}
			}
			else if(ch == '>')
			{
				ch = inputFile[++k];;
				if(ch == '=') // ">=" token
				{
					buffer[i] = ch;
					i++;

					buffer[i] = '\0';

					// Add lexeme to table
					lexeme_table[j].name = malloc(sizeof(char) * (i+1));
					strcpy(lexeme_table[j].name, buffer);
					lexeme_table[j].token = ssym[buffer[0] * 2 + buffer[1]];
					lexeme_table[j].error = error;

					j++;
				}
				else // ">" token
				{
					buffer[i] = '\0';

					// Add lexeme to table
					lexeme_table[j].name = malloc(sizeof(char) * (i+1));
					strcpy(lexeme_table[j].name, buffer);
					lexeme_table[j].token = gtrsym;
					lexeme_table[j].error = error;

					j++;
				}
			}
			else if(ch == ':')
			{
				ch = inputFile[++k];;
				if(ch == '=') // ":=" token
				{
					buffer[i] = ch;
					i++;

					buffer[i] = '\0';

					// Add lexeme to table
					lexeme_table[j].name = malloc(sizeof(char) * (i+1));
					strcpy(lexeme_table[j].name, buffer);
					lexeme_table[j].token = ssym[buffer[0] * 2 + buffer[1]];
					lexeme_table[j].error = error;

					j++;
				}
				else // Invalid symbol
				{
					buffer[i] = '\0';

					// Add lexeme to table
					lexeme_table[j].name = malloc(sizeof(char) * (i+1));
					strcpy(lexeme_table[j].name, buffer);
					lexeme_table[j].token = nulsym;
					lexeme_table[j].error = invalidsym;

					printf("ERROR: invalid identifier\n");
					exit(EXIT_FAILURE);

					j++;
				}
			}
			else if(ch == '/')
			{
				ch = inputFile[++k];;
				if(ch == '*') // Start comment
				{
					buffer[i] = ch;
					i++;

					buffer[i] = '\0';

					// Add lexeme to table
					lexeme_table[j].name = malloc(sizeof(char) * (i+1));
					strcpy(lexeme_table[j].name, buffer);
					lexeme_table[j].token = ssym[buffer[0] * 2 + buffer[1]];
					lexeme_table[j].error = error;

					j++;

					while((ch = inputFile[++k]) != '\0')
					{
						if(ch == '*')
						{
							ch = inputFile[++k];;
							if(ch == '/') // Exit comment
							{
								char temp = buffer[0];
								buffer[0] = buffer[1];
								buffer[1] = temp;

								// Add lexeme to table
								lexeme_table[j].name = malloc(sizeof(char) * (i+1));
								strcpy(lexeme_table[j].name, buffer);
								lexeme_table[j].token = ssym[buffer[0] * 2 + buffer[1]];
								lexeme_table[j].error = error;

								j++;
								break;
							}
							else
							{
								continue;
							}

						}
					}
				}
				else // "/" token
				{
					buffer[i] = '\0';

					// Add lexeme to table
					lexeme_table[j].name = malloc(sizeof(char) * (i+1));
					strcpy(lexeme_table[j].name, buffer);
					lexeme_table[j].token = slashsym;
					lexeme_table[j].error = error;

					j++;

					continue;
				}
			}
			else // Invalid symbol
			{
				buffer[i] = '\0';

				// Add lexeme to table
				lexeme_table[j].name = malloc(sizeof(char) * (i+1));
				strcpy(lexeme_table[j].name, buffer);
				lexeme_table[j].token = nulsym;
				lexeme_table[j].error = invalidsym;

				printf("ERROR: invalid symbol\n");
				exit(EXIT_FAILURE);

				j++;
			}
			ch = inputFile[++k];
		}
	}

	if(flag)
	{
		printf("\nLexeme List:\n");
		for(i = 0; i < j; i++)
		{
			l = lexeme_table[i];
			if(l.error == 0)
			{
				if(l.token == identsym)
					printf("%d %s ", l.token, l.name);

				else if(l.token == numbersym)
					printf("%d %d ", l.token, l.value);

				else
					printf("%d ", l.token);

				if((i + 1) < j)
					printf("| ");
			}
		}
		printf("\n\n");
	}

	return lexeme_table;
}
