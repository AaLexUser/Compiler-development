#pragma once

#include <stdio.h>

/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(char *s, ...);

/* symbol table */
struct symbol
{ /* a variable name */
    char *name;
    int offset;
};

/* simple symtab of fixes size */
#define NHASH 9997
struct symbol symtab[NHASH];

struct symbol *lookup(char *);
struct symbol * define_sym(char *);

typedef enum ntype {
    /* keywords */
    NT_IF, NT_WHILE, NT_FOR,
    /* comparison ops */
    NT_EQ, NT_NEQ, NT_LT, NT_LTE, NT_GT, NT_GTE,
    /* arithmetic ops */
    NT_ADD, NT_SUB, NT_MUL, NT_DIV,
    /* logical ops */
    NT_AND, NT_OR, NT_NOT,
    /* other */
    NT_NUM, NT_REF, NT_ASGN, NT_LIST, NT_UMINUS
} ntype_t;

static char* node2str[] ={
    [NT_IF] = "IF",
    [NT_WHILE] = "WHILE",
    [NT_FOR] = "FOR",
    [NT_EQ] = "==",
    [NT_NEQ] = "!=",
    [NT_LT] = "<",
    [NT_LTE] = "<=",
    [NT_GT] = ">",
    [NT_GTE] = ">=",
    [NT_ADD] = "+",
    [NT_SUB] = "-",
    [NT_MUL] = "*",
    [NT_DIV] = "/",
    [NT_AND] = "AND",
    [NT_OR] = "OR",
    [NT_NOT] = "NOT",
    [NT_NUM] = "NUM",
    [NT_REF] = "REF",
    [NT_ASGN] = "ASGN",
    [NT_LIST] = "LIST",
    [NT_UMINUS] = "UMINUS"
};


/* nodes in the abstract syntax tree */
struct ast
{
    ntype_t nodetype;
    struct ast *l;
    struct ast *r;
};

struct numval
{
    ntype_t nodetype; /* type K for constant */
    int number;
};

struct flow
{
    ntype_t nodetype;     /* type I or W */
    struct ast *cond; /* condition */
    struct ast *tl;   /* then or do list */
    struct ast *el;   /* optional else list */
};

struct for_loop
{
    ntype_t nodetype;
    struct ast *init;
    struct ast *cond;
    struct ast *inc;
    struct ast *tl;
};

struct symref
{
    ntype_t nodetype; /* type N */
    struct symbol *s;
};

struct symasgn
{
    ntype_t nodetype; /* type = */
    struct symbol *s;
    struct ast *v; /* value */
};

/* build an AST */
struct ast *newast(ntype_t nodetype, struct ast *l, struct ast *r);
struct ast *newcmp(ntype_t cmptype, struct ast *l, struct ast *r);
struct ast *newflow(ntype_t nodetype, struct ast *cond, struct ast *tl, struct ast *el);
struct ast* newfor(struct ast* init, struct ast* cond, struct ast* inc, struct ast* tl);
struct ast *newnum(int d);
struct ast *newref(char *s);
struct ast *newasgn(char *s, struct ast *v);

/* print an AST */
void print_ast(FILE *stream, struct ast *ast, int level);
void print_asm(FILE *stream, struct ast *ast);

/* delete and free an AST */
void treefree(struct ast *);

/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(char *s, ...);
