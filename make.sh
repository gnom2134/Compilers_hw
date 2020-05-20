flex my_lexer.l
bison my_parser.y -d
gcc my_functions.c my_parser.tab.c lex.yy.c -o comp