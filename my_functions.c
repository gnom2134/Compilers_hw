#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "my_functions.h"

static unsigned symbol_hash(char *sym) {
    unsigned int hash = 0;
    unsigned character;

    while(character = *sym++) {
        hash = hash * 9 ^ character;
    }

    return hash;
}

struct symbol* lookup(char* sym) {
    struct symbol *sp = &symbol_table[symbol_hash(sym) % N_HASH];
    int s_count = N_HASH;

    while(--s_count >= 0) {
        if (sp->name && !strcmp(sp->name, sym)) {
            return sp;
        }

        if (!sp->name) {
            sp->name = strdup(sym);
            sp->value = 0;
            return sp;
        }

        if (++sp >= symbol_table + N_HASH) {
            sp = symbol_table;
        }
    }

    yyerror("symbol table overflow\n");
    abort();
}

struct symbol* null_lookup(char* sym) {
    struct symbol *sp = &symbol_table[symbol_hash(sym) % N_HASH];
    int s_count = N_HASH;

    while(--s_count >= 0) {
        if (sp->name && !strcmp(sp->name, sym)) {
            return sp;
        }

        if (!sp->name) {
            return NULL;
        }

        if (++sp >= symbol_table + N_HASH) {
            sp = symbol_table;
        }
    }

}

struct ast_node* new_ast(int nodetype, struct ast_node *l, struct ast_node *r) {
    struct ast_node *a = malloc(sizeof(struct ast_node));

    if(!a) {
        yyerror("out of space");
        exit(0);
    }
    a->nodetype = nodetype;
    a->l = l;
    a->r = r;
    return a;
}

struct ast_node* new_number(double d) {
    struct numval *a = malloc(sizeof(struct numval));

    if(!a) {
        yyerror("out of space");
        exit(0);
    }
    a->nodetype = 'K';
    a->number = d;
    return (struct ast_node *)a;
}

struct ast_node* new_assign(struct symbol *s, struct ast_node *v) {
    struct symasgn *a = malloc(sizeof(struct symasgn));

    if(!a) {
        yyerror("out of space");
        exit(0);
    }
    a->nodetype = '=';
    a->s = s;
    a->v = v;
    return (struct ast_node *)a;
}

struct ast_node* new_reference(struct symbol *s) {
    struct symref *a = malloc(sizeof(struct symref));

    if(!a) {
        yyerror("out of space");
        exit(0);
    }
    a->nodetype = 'N';
    a->s = s;
    return (struct ast_node *)a;
}

struct ast_node* new_flow(int nodetype, struct ast_node* cond, struct ast_node* operators) {
    struct flow *a = malloc(sizeof(struct flow));
    if(!a) {
        yyerror("out of space");
        exit(0);
    }
    a->nodetype = nodetype;
    a->cond = cond;
    a->tl = operators;
    return (struct ast_node *)a;
}

struct symlist* new_symlist(struct symbol* sym, struct symlist* next)
{
    struct symlist *sl = malloc(sizeof(struct symlist));

    if(!sl) {
        yyerror("out of space");
        exit(0);
    }
    sl->sym = sym;
    sl->next = next;
    return sl;
}

struct ast_node* new_var(struct symlist *syms) {
    struct symlist* head = syms;
    while (syms) {
        struct symbol* sym = lookup(syms->sym->name);
        sym->name = syms->sym->name;
        sym->value = 0.0;
        syms = syms->next;
    }
    struct symvarlist* node = malloc(sizeof(struct symvarlist));
    node->nodetype = 'V';
    node->syms = head;
    return (struct ast_node*) node;
}

double eval(struct ast_node* a) {
    double v;

    if(!a) {
        yyerror("internal error, null eval");
        return 0.0;
    }

    switch(a->nodetype) {
        case 'K': v = ((struct numval *)a)->number; break;

        case 'N': v = ((struct symref *)a)->s->value; break;

        case 'V': break;
        case 'P': eval(a->l); v = eval(a->r); break;
        case 'O': eval(a->l); v = eval(a->r); break;

        case '=': v = ((struct symasgn *)a)->s->value =
                    eval(((struct symasgn *)a)->v); break;

        case '+': v = eval(a->l) + eval(a->r); break;
        case '-': v = eval(a->l) - eval(a->r); break;
        case '*': v = eval(a->l) * eval(a->r); break;
        case '/': v = eval(a->l) / eval(a->r); break;
        case 'M': v = -eval(a->l); break;
        case 'n': v = !(eval(a->l)); break;
        case 'p': v = eval(a->l); printf("%f\n", v); break;

        case '>': v = (eval(a->l) > eval(a->r))? 1 : 0; break;
        case '<': v = (eval(a->l) < eval(a->r))? 1 : 0; break;
        case 'E': v = (eval(a->l) == eval(a->r))? 1 : 0; break;

        case 'W':
            v = 0.0;

            if( ((struct flow *)a)->tl) {
                while( eval(((struct flow *)a)->cond) != 0)
                    v = eval(((struct flow *)a)->tl);
            }
            break;

        case 'L': eval(a->l); v = eval(a->r); break;

        default: printf("internal error: bad node %c\n", a->nodetype);
    }
    return v;
}

void print_ast(struct ast_node* a, int line_id) {
    printf("line id:%i - ", line_id);
    int new_line_id1 = line_id * 2;
    int new_line_id2 = line_id * 2 + 1;
    switch(a->nodetype) {
        case 'K': {
            printf("number %f\n", ((struct numval *) a)->number);
            break;
        }

        case 'P': {
            printf("program with var declaration on line with id %i and operators on line with id %i\n", new_line_id1, new_line_id2);
            print_ast(a->l, new_line_id1);
            print_ast(a->r, new_line_id2);
            break;
        }

        case 'O': {
            printf("operator line with commands in line with id %i and nex operator in line with id %i\n", new_line_id1, new_line_id2);
            print_ast(a->l, new_line_id1);
            print_ast(a->r, new_line_id2);
            break;
        }

        case 'N': {
            printf("reference to %s\n", ((struct symref *)a)->s->name);
            break;
        }

        case 'V' : {
            printf("Variables definition part\n");
            break;
        }

        case '=': {
            printf("assigning value from line with id %i to %s\n", new_line_id1, ((struct symasgn *) a)->s->name);
            print_ast(((struct symasgn *)a)->v, new_line_id1);
            break;
        }

        case '+': {
            printf("adding two numbers from line with id %i and line with id %i\n", new_line_id1, new_line_id2);
            print_ast(a->l, new_line_id1);
            print_ast(a->r, new_line_id2);
            break;
        }
        case '-': {
            printf("subtracting two numbers from line with id %i and line with id %i\n", new_line_id1, new_line_id2);
            print_ast(a->l, new_line_id1);
            print_ast(a->r, new_line_id2);
            break;
        }
        case '*': {
            printf("multiplying two numbers from line with id %i and line with id %i\n", new_line_id1, new_line_id2);
            print_ast(a->l, new_line_id1);
            print_ast(a->r, new_line_id2);
            break;
        }
        case '/': {
            printf("dividing two numbers from line with id %i and line with id %i\n", new_line_id1, new_line_id2);
            print_ast(a->l, new_line_id1);
            print_ast(a->r, new_line_id2);
            break;
        }
        case 'M': {
            printf("unary minus to from line with id %i\n", new_line_id1);
            print_ast(a->l, new_line_id1);
            break;
        }
        case 'n': {
            printf("unary not to from line with id %i\n", new_line_id1);
            print_ast(a->l, new_line_id1);
            break;
        }

        case '>': {
            printf("checking if number from line with id %i is greater than number from line with id %i\n", new_line_id1, new_line_id2);
            print_ast(a->l, new_line_id1);
            print_ast(a->r, new_line_id2);
            break;
        }
        case '<': {
            printf("checking if number from line with id %i is less than number from line with id %i\n", new_line_id1, new_line_id2);
            print_ast(a->l, new_line_id1);
            print_ast(a->r, new_line_id2);
            break;
        }
        case 'E': {
            printf("checking if number from line with id %i is equal to number from line with id %i\n", new_line_id1, new_line_id2);
            print_ast(a->l, new_line_id1);
            print_ast(a->r, new_line_id2);
            break;
        }

        case 'W': {
            printf("loop with condition in line with id %i and body in line with id %i\n", new_line_id1, new_line_id2);
            print_ast(((struct flow *) a)->cond, new_line_id1);
            if (((struct flow *) a)->cond) {
                print_ast(((struct flow *) a)->tl, new_line_id2);
            }
            break;
        }

        case 'p': {
            printf("printing expression from the line with id %i\n", new_line_id1);
            print_ast(a->l, new_line_id1);
            break;
        }

        case 'L': {
            printf("symlist with contents in line with id %i and line with id %i\n", new_line_id1, new_line_id2);
            print_ast(a->l, new_line_id1);
            print_ast(a->r, new_line_id2);
            break;
        }

        default: {
            printf("internal error: bad node %c\n", a->nodetype);
        }
    }
}

void treefree(struct ast_node * a) {
    switch(a->nodetype) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '<':
        case '>':
        case 'E':
        case 'L':
        case 'O':
        case 'P':
            treefree(a->r);
        case 'M':
        case 'p':
        case 'n':
            treefree(a->l);
            free(a);
            break;
        case 'K': case 'N':
            free(a);
            break;
        case '=':
            treefree( ((struct symasgn *)a)->v);
            free(a);
            break;
        case 'W':
            free( ((struct flow *)a)->cond);
            if( ((struct flow *)a)->tl) treefree( ((struct flow *)a)->tl);
            free(a);
            break;
        case 'V':
            if (((struct symvarlist *)a)->syms) {
                symlistfree(((struct symvarlist *)a)->syms);
            }
            free( (struct symvarlist *)a);
            break;
        default: printf("internal error: free bad node %c\n", a->nodetype);
    }
}

void symlistfree(struct symlist *sl) {
    struct symlist *nsl;

    while(sl) {
        nsl = sl->next;
        free(sl);
        sl = nsl;
    }
}

void yyerror(char *s, ...)
{
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}

int main() {
    return yyparse();
}