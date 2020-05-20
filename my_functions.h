#define N_HASH 9997

struct symbol {
    char* name;
    double value;
};

struct symbol symbol_table[N_HASH];

struct ast_node {
    int nodetype;
    struct ast* l;
    struct ast* r;
};

struct symlist {
    struct symbol *sym;
    struct symbol_list *next;
};

struct flow {
    int nodetype;
    struct ast_node* cond;
    struct ast_node* tl;
};

struct numval {
    int nodetype;
    double number;
};

struct symref {
    int nodetype;
    struct symbol* s;
};

struct symasgn {
    int nodetype;
    struct symbol* s;
    struct ast_node* v;
};

struct ast_node* new_ast(int node_type, struct ast *l, struct ast *r);

struct ast_node* new_number(double d);

struct ast_node* new_assign(struct symbol *s, struct ast_node *v);

struct ast_node* new_reference(struct symbol *s);

struct ast_node* new_flow(int node_type, struct ast_node* cond, struct ast_node* operators);

struct symlist* new_symlist(struct symbol* sym, struct symlist* next);

double eval(struct ast_node *);

void treefree(struct ast_node *);

void print_ast(struct ast_node* a, int line_id);

extern int yylineno;
void yyerror(char *s, ...);