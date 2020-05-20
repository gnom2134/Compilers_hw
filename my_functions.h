#define N_HASH 9997

struct symbol {
    char* name;
    double value;
    int check;
};

struct symbol symbol_table[N_HASH];

static unsigned symbol_hash(char *sym);

struct symbol* lookup(char* sym);

struct symbol* null_lookup(char* sym);

struct ast_node {
    int nodetype;
    struct ast_node* l;
    struct ast_node* r;
};

struct symlist {
    struct symbol *sym;
    struct symlist *next;
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

struct symvarlist {
    int nodetype;
    struct symlist* syms;
};

struct ast_node* new_ast(int node_type, struct ast_node *l, struct ast_node *r);

struct ast_node* new_number(double d);

struct ast_node* new_assign(struct symbol *s, struct ast_node *v);

struct ast_node* new_reference(struct symbol *s);

struct ast_node* new_flow(int node_type, struct ast_node* cond, struct ast_node* operators);

struct ast_node* new_var(struct symlist *syms);

struct symlist* new_symlist(struct symbol* sym, struct symlist* next);

void dodef(struct symbol *name, struct symlist *syms, struct ast_node *stmts);

double eval(struct ast_node *);

void treefree(struct ast_node *);

void symlistfree(struct symlist *sl);

void print_ast(struct ast_node* a, int line_id);

extern int yylineno;
void yyerror(char *s, ...);