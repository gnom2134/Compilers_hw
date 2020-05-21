%{#include <stdio.h>#include <stdlib.h>#include "my_functions.h"%}%union {	struct ast_node *a;	int d;	struct symbol *s;	struct symlist *sl;	int i;}%token <d> CNT%token <s> NAME%token NOT EQL%token WHILE DO VAR BGN END PRINT%right '='%left '+' '-'%left '*' '/'%left '<' '>' EQL%nonassoc UMINUS NOT%type <a> program var_declaration operator assign exp operand loop_operator compound_operator operators_list pp%type <sl> symlist%start program_wrapper%%program_wrapper: program			{							printf("Program finished with result code: %i\n", eval($1));							print_ast($1, 1);							treefree($1);						};program: var_declaration operators_list 	{$$ = new_ast('P', $1, $2);};var_declaration: VAR symlist 			{$$ = new_var($2);};symlist: NAME 					{$1->check = 1; $$ = new_symlist($1, NULL);}	| NAME ',' symlist 			{$1->check = 1; $$ = new_symlist($1, $3);};operators_list: operator			{$$ = $1;}	| operator operators_list		{							if ($2 != NULL) {								$$ = new_ast('O', $1, $2);							} else {								$$ = $1;							}						};operator: assign				{$$ = $1;}	| loop_operator				{$$ = $1;}	| compound_operator			{$$ = $1;}	| pp 					{$$ = $1;};pp: PRINT '(' exp ')'				{$$ = new_ast('p', $3, NULL);}assign: NAME '=' exp				{$$ = new_assign($1, $3);};exp: '(' exp ')'				{$$ = $2;}	| operand				{$$ = $1;}	| '-' exp %prec UMINUS			{$$ = new_ast('M', $2, NULL);}	| NOT exp 				{$$ = new_ast('n', $2, NULL);}	| exp '-' exp				{$$ = new_ast('-', $1, $3);}	| exp '+' exp				{$$ = new_ast('+', $1, $3);}	| exp '*' exp				{$$ = new_ast('*', $1, $3);}	| exp '/' exp				{$$ = new_ast('/', $1, $3);}	| exp '<' exp				{$$ = new_ast('<', $1, $3);}	| exp '>' exp				{$$ = new_ast('>', $1, $3);}	| exp EQL exp				{$$ = new_ast('E', $1, $3);};operand: NAME 					{$$ = new_reference($1);}	| CNT 					{$$ = new_number($1);};loop_operator: WHILE exp DO operator		{$$ = new_flow('W', $2, $4);};compound_operator: BGN operators_list END	{$$ = $2;};%%