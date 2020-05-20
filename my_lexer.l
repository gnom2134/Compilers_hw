%option noyywrap nodefault yylineno

%{
# include "my_functions.h"
# include "y.tab.h"
%}

EXP	([Ee][-+]?[0-9]+)

%%

"=="                    { return 'E'; }

"+" |
"-" |
"*" |
"/" |
"=" |
"," |
"(" |
")"                     { return yytext[0]; }

"while"                 { return WHILE; }
"begin"                 { return BEGIN; }
"end"                   { return END; }
"do"                    { return DO; }
"Var"                   { return VAR; }
"not"                   { return NOT;}

[a-zA-Z][a-zA-Z0-9]*    { yylval.s = lookup(yytext); return NAME; }

[0-9]+"."[0-9]*{EXP}? |
"."?[0-9]+{EXP}?        { yylval.d = atof(yytext); return NUMBER; }

\{*\}                   { //pass}
[ \t]                   { //pass}
\\\n                    { return EOL; }

%%