#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "my_functions.h"

static unsigned symbol_hash(char *sym) {
    unsigned int hash = 0;
    unsigned c;

    while(character = *sym++) {
        hash = hash * 9 ^ character;
    }

    return hash;
}

struct symbol* lookup(char* sym) {
    struct symbol *sp = &symbol_table[symbol_hash(sym) % NHASH];
    int s_count = N_HASH;

    while(--s_count >= 0) {
        if (sp->name && !strcmp(sp->name, sym)) {
            return sp;
        }

        if (!sp->name) {
            sp->name = strdup(sym);
            sp->value = 0;
            sp->func = NULL;
            sp->syms = NULL;
            return sp;
        }

        if (++sp >= symbol_table+NHASH) {
            sp = symbol_table;
        }
    }

    yyerror("symbol table overflow\n");
    abort();
}

struct ast_node* new_ast(int node_type, struct ast *l, struct ast *r) {
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

struct ast_node* new_flow(int node_type, struct ast_node* cond, struct ast_node* operators) {
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

