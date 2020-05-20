%option noyywrap nodefault yylineno

%{
#include "my_functions.h"
#include "my_parser.tab.h"
%}

EXP	([Ee][-+]?[0-9]+)

%%
"=="                    { return EQL; }
"+" |
"-" |
"*" |
"/" |
"=" |
"," |
"<" |
">" |
"(" |
")"                     { return yytext[0]; }
"print"                 { return PRINT; }
"while"                 { return WHILE; }
"begin"[ \t]*["\n"]*    { return BGN; }
[ \t]*["\n"]*"end"      { return END; }
"do"                    { return DO; }
"Var"                   { return VAR; }
"not"                   { return NOT; }
[a-zA-Z][a-zA-Z0-9]*    {
                            yylval.s = lookup(yytext);
                            return NAME;
                        }
[0-9]+"."[0-9]*{EXP}? |
"."?[0-9]+{EXP}?        {
                            yylval.d = atof(yytext);
                            return CNT;
                        }
"{".*"}"                /* pass */
[ \t]                   /* pass */
"\n"                    { return SEP; }
%%