%{#include <stdio.h>#include <stdlib.h>#include "my_functions.h"%}%union {	struct ast *a;	double d;	struct symbol *s;	struct symbol_list *sl;	int fn;}%token <d> NUMBER%token <s> NAME%token <fn> FUNC%token EOL NOT%token WHILE DO VAR BEGIN END%nonassoc <fn> CMP%right '='%left '+' '-'%left '*' '/'%nonassoc UMINUS%type <a> operation stmt list operators_list%type <sl> symlist%start program%%program: var_declaration calculation 		{$$ = new_ast('P', $2, $3);};var_decalration: VAR symlist 			{$$ = $2;};symlist: 					{$$ = NULL;}	| NAME 					{$$ = $1;}	| NAME "," symlist 			{							if ($3 != NULL) {								$$ = new_ast('L', $1, $3);							} else {								$$ = $1;							}						};calculation: operators_list			{$$ = $1;};operators_list: operator			{$$ = $1;}	| opperator operators_list		{							if ($2 != NULL) {								$$ = new_ast('O', $1, $2);							} else {								$$ = $1;							}						};operator: assign				{$$ = $1;}	| complicated_operator			{$$ = $1;};assing: NAME "=" exp				{$$ = new_assign($1, $3);};exp: un_operation subexp			{$$ = newast($1, $2, NULL);}	| subexp				{$$ = $1;};subexp: "(" exp ")"				{$$ = $2;}	| operand				{$$ = $1;}	| subexp bin_operation subexp		{$$ = new_ast($2, $1, $3);};un_operation: "-" %prec UMINUS			{$$ = 'M';}	| NOT 					{$$ = 'N';};bin_operation: "-"				{$$ = '-';}	| "+" 					{$$ = '+';}	| "*"					{$$ = '*';}	| "/"					{$$ = '/';}	| "<"					{$$ = '<';}	| ">"					{$$ = '>';}	| "=="					{$$ = 'E';};operand: NAME 					{$$ = new_reference($1);}	| const 				{$$ = new_number($1);};complicated_operator: loop_operator		{$$ = $1;}	| compound_operator			{$$ = $1;};loop_operator: WHILE exp DO operator		{$$ = new_flow('W', $2, $4);};compound_operator: BEGIN operators_list END	{$$ = $2;};%%