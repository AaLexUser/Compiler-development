/**
 * @file ast.c
 * This module implements the functions for creating and managing an abstract syntax tree (AST) used in a compiler
 * design.
 *
 * Each function is designed to handle specific types of nodes within the AST, ensuring proper memory management and
 * tree structure maintenance.
 */

#include "ast.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Computes a hash value for a given symbol string.
 *
 * This function takes a string representing a symbol and computes a hash value
 * for it using a simple hashing algorithm. The hash is computed by iterating
 * over each character in the string, multiplying the current hash value by 9,
 * and then XORing it with the character's ASCII value.
 *
 * @param sym A pointer to the null-terminated string representing the symbol.
 * @return An unsigned integer representing the computed hash value.
 */
static unsigned int symhash(char *sym)
{
    unsigned int hash = 0;
    unsigned c;

    while ((c = (*sym++)))
        hash = hash * 9 ^ c;

    return hash;
}

/**
 * @brief Looks up a symbol in the symbol table.
 *
 * This function searches for a symbol in the symbol table. If the symbol is
 * found, a pointer to the corresponding symbol structure is returned. If the
 * symbol is not found, a new entry is created in the symbol table.
 *
 * @param sym The symbol to look up.
 * @return A pointer to the symbol structure corresponding to the symbol.
 *         If the symbol table is full, the function will call `abort()`.
 */
struct symbol *lookup(char *sym)
{
    struct symbol *sp = &symtab[symhash(sym) % NHASH];
    int scount = NHASH;

    while (--scount >= 0)
    {
        if (sp->name && !strcmp(sp->name, sym))
        {
            return sp;
        }

        if (!sp->name)
            return NULL;

        if (++sp >= symtab + NHASH)
            sp = symtab; /* try the next entry */
    }
    yyerror("Symbol not found\n");
    abort(); /* tried them all, table is full */
}

struct symbol *define_sym(char *sym)
{
    struct symbol *sp = &symtab[symhash(sym) % NHASH];
    int scount = NHASH;

    while (--scount >= 0)
    {
        if (sp->name && !strcmp(sp->name, sym))
        {
            return sp;
        }
        /* new entry */
        if (!sp->name)
        {
            sp->name = strdup(sym);
            return sp;
        }

        if (++sp >= symtab + NHASH)
            sp = symtab; /* try the next entry */
    }
    yyerror("symbol table overflow\n");
    abort(); /* tried them all, table is full */
}

/**
 * Creates a new abstract syntax tree (AST) node.
 *
 * @param nodetype The type of the node.
 * @param l Pointer to the left child node.
 * @param r Pointer to the right child node.
 * @return Pointer to the newly created AST node.
 */
struct ast *newast(ntype_t nodetype, struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = nodetype;
    a->l = l;
    a->r = r;
    return a;
}

/**
 * Creates a new number node for the abstract syntax tree (AST).
 *
 * @param d The integer value to be stored in the number node.
 * @return A pointer to the newly created number node casted as a struct ast
 * pointer.
 */
struct ast *newnum(int d)
{
    struct numval *a = malloc(sizeof(struct numval));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = NT_NUM;
    a->number = d;
    return (struct ast *)a;
}

/**
 * Creates a new comparison node for the abstract syntax tree (AST).
 *
 * @param cmptype The type of comparison
 * @param l A pointer to the left-hand side AST node.
 * @param r A pointer to the right-hand side AST node.
 * @return A pointer to the newly created comparison node.
 */
struct ast *newcmp(ntype_t cmptype, struct ast *l, struct ast *r)
{
    struct ast *a = malloc(sizeof(struct ast));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = cmptype;
    a->l = l;
    a->r = r;
    return a;
}

/**
 * Creates a new flow control node for the abstract syntax tree (AST).
 *
 * @param nodetype The type of flow control
 * @param cond A pointer to the condition AST node.
 * @param tl A pointer to the true branch AST node (executed if the condition
 * is true).
 * @param el A pointer to the else branch AST node (executed if the condition
 * is false).
 * @return A pointer to the newly created flow control node casted as a struct
 * ast pointer.
 */
struct ast *newflow(ntype_t nodetype, struct ast *cond, struct ast *tl, struct ast *el)
{
    struct flow *a = malloc(sizeof(struct flow));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = nodetype;
    a->cond = cond;
    a->tl = tl;
    a->el = el;
    return (struct ast *)a;
}

/**
 * Creates a new reference to a symbol.
 *
 * @param s A pointer to the symbol to be referenced.
 * @return A pointer to the newly created ast structure.
 */
struct ast *newref(char *s)
{
    struct symbol *sym = lookup(s);
    if (sym == NULL)
    {
        yyerror("NameError: name '%s' is not defined", s);
        exit(0);
    }
    struct symref *a = malloc(sizeof(struct symref));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = NT_REF;
    a->s = sym;
    return (struct ast *)a;
}

/**
 * Creates a new assignment AST node.
 *
 * @param s A pointer to the symbol to be assigned.
 * @param v A pointer to the AST node representing the value to be assigned.
 * @return A pointer to the newly created assignment AST node.
 */
struct ast *newasgn(char *s, struct ast *v)
{

    struct symbol *sym = define_sym(s);
    struct symasgn *a = malloc(sizeof(struct symasgn));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = NT_ASGN;
    a->s = sym;
    a->v = v;
    return (struct ast *)a;
}

struct ast* newfor(struct ast* init, struct ast* cond, struct ast* inc, struct ast* tl)
{
    struct for_loop* a = malloc(sizeof(struct for_loop));

    if (!a)
    {
        yyerror("out of space");
        exit(0);
    }

    a->nodetype = NT_FOR;
    a->init = init;
    a->cond = cond;
    a->inc = inc;
    a->tl = tl;
    return (struct ast *)a;
}


/**
 * @brief Frees the memory allocated for the abstract syntax tree (AST).
 *
 * This function recursively frees all the nodes of the AST starting from the node passed to it.
 * It handles different types of nodes according to their nodetype, which determines the structure
 * and the number of child nodes they have.
 *
 * @param a Pointer to the node of the AST to be freed.
 */
void treefree(struct ast *a)
{
    switch (a->nodetype)
    {
        // clang-format off
        /* Nodes with two subtrees */
        case NT_ADD:
        case NT_SUB:
        case NT_MUL:
        case NT_DIV:
        case NT_GT: case NT_GTE: case NT_LT: case NT_LTE: case NT_EQ: case NT_NEQ: // comparisons
        case NT_AND: case NT_OR: // logical operators
        case NT_LIST:
            treefree(a->r);
            /* Nodes with one subtree */
        case NT_UMINUS:
        case NT_NOT:
            treefree(a->l);
            /* Nodes with no subtree */
        case NT_NUM: case NT_REF:
            break;
        case NT_ASGN:  // Assignment node, free the value
            free(((struct symasgn *)a)->v);
            break;
        case NT_IF: case NT_WHILE:  // If and While nodes, condition and branches
            free(((struct flow *)a)->cond);
            if(((struct flow *)a)->tl) treefree(((struct flow *)a)->tl);
            if(((struct flow *)a)->el) treefree(((struct flow *)a)->el);
            break;
        case NT_FOR:  // For node, free the init, condition, increment and body
            if(((struct for_loop *)a)->init) treefree(((struct for_loop *)a)->init);
            if(((struct for_loop *)a)->cond) treefree(((struct for_loop *)a)->cond);
            if(((struct for_loop *)a)->inc) treefree(((struct for_loop *)a)->inc);
            if(((struct for_loop *)a)->tl) treefree(((struct for_loop *)a)->tl);
            break;
        
        default: printf("internal error: free bad node %c\n", a->nodetype);
        // clang-format on
    }
}

static void print_indent(FILE *stream, int level)
{
    char *indent = malloc(sizeof(char) * (level * 2 + 1));
    memset(indent, ' ', level * 2 + 1);
    indent[level * 2] = '\0';
    fprintf(stream, "%s", indent);
    free(indent);
}

static void print_node(FILE *stream, int level, char *arguments, ...)
{
    va_list args;
    va_start(args, arguments);
    print_indent(stream, level);
    vfprintf(stream, arguments, args);
    va_end(args);
}

int seq_reg_num = 1;
int if_num = -1;
int while_num = -1;

int next_reg()
{
    seq_reg_num = seq_reg_num != 31 ? seq_reg_num + 1 : 1;
    return seq_reg_num;
}
void free_reg()
{
    seq_reg_num = seq_reg_num != 1 ? seq_reg_num - 1 : 31;
}

void interpret_prog(FILE *stream, struct ast *ast, int reg)
{
    if (!ast)
    {
        return;
    }
    int rs2, temp1, temp2 = 0;
    int num = 0;
    switch (ast->nodetype)
    {
    case NT_LIST:
        interpret_prog(stream, ast->l, reg);
        interpret_prog(stream, ast->r, reg);
        break;
    case NT_NUM:
        fprintf(stream, "li x%d, %d\n", reg, ((struct numval *)ast)->number);
        break;
    case NT_REF:
        fprintf(stream, "lw x%d, x0, %d\n", reg, ((struct symref *)ast)->s->offset);
        break;
    case NT_ASGN:
        interpret_prog(stream, ((struct symasgn *)ast)->v, reg);
        fprintf(stream, "sw x0, %d, x%d\n", ((struct symasgn *)ast)->s->offset, reg);
        break;
    case NT_ADD:
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "add x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case NT_SUB:
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "sub x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case NT_MUL:
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "mul x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case NT_DIV:
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "div x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case NT_GT: // >
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        temp1 = next_reg();
        temp2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "sne x%d, x%d, x%d\n", temp1, reg, rs2);
        fprintf(stream, "sge x%d, x%d, x%d\n", temp2, reg, rs2);
        fprintf(stream, "and x%d, x%d, x%d\n", reg, temp1, temp2);
        free_reg();
        free_reg();
        free_reg();
        break;
    case NT_LT: // <
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "slt x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case NT_NEQ: // !=
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "sne x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case NT_EQ: // ==
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "seq x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case NT_GTE: // >=
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "sge x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case NT_LTE: // <=
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        temp1 = next_reg();
        temp2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "seq x%d, x%d, x%d\n", temp1, reg, rs2);
        fprintf(stream, "slt x%d, x%d, x%d\n", temp2, reg, rs2);
        fprintf(stream, "or x%d, x%d, x%d\n", reg, temp1, temp2);
        free_reg();
        free_reg();
        free_reg();
        break;
    case NT_AND:
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "and x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case NT_OR:
        interpret_prog(stream, ast->l, reg);
        rs2 = next_reg();
        interpret_prog(stream, ast->r, rs2);
        fprintf(stream, "or x%d, x%d, x%d\n", reg, reg, rs2);
        free_reg();
        break;
    case NT_NOT:
        interpret_prog(stream, ast->l, reg);
        fprintf(stream, "xori x%d, x%d, 1\n", reg, reg);
        break;
    case NT_UMINUS:
        interpret_prog(stream, ast->l, reg);
        fprintf(stream, "sub x%d, x0, x%d\n", reg, reg);
        break;
    case NT_IF:
        num = ++if_num;
        temp1 = next_reg();
        interpret_prog(stream, ((struct flow *)ast)->cond, temp1);
        if (((struct flow *)ast)->el != NULL)
        {
            fprintf(stream, "beq x%d, x0, ELSE%d\n", temp1, num);
        }
        else
        {
            fprintf(stream, "beq x%d, x0, ENDIF%d\n", temp1, num);
        }
        free_reg();
        interpret_prog(stream, ((struct flow *)ast)->tl, reg);
        fprintf(stream, "jal x%d, ENDIF%d\n", reg, num);
        if (((struct flow *)ast)->el != NULL)
        {
            fprintf(stream, "ELSE%d:\n", num);
            interpret_prog(stream, ((struct flow *)ast)->el, reg);
        }
        fprintf(stream, "ENDIF%d:\n", num);
        break;
    case NT_WHILE:
        num = ++while_num;
        fprintf(stream, "WHILE%d:\n", num);
        temp1 = next_reg();
        interpret_prog(stream, ((struct flow *)ast)->cond, temp1);
        fprintf(stream, "beq x%d, x0, ENDWHILE%d\n", temp1, num);
        free_reg();
        interpret_prog(stream, ((struct flow *)ast)->tl, reg);
        fprintf(stream, "jal x%d, WHILE%d\n", reg, num);
        fprintf(stream, "ENDWHILE%d:\n", num);
        break;
    case NT_FOR:
        num = ++while_num;
        interpret_prog(stream, ((struct for_loop *)ast)->init, reg);
        fprintf(stream, "WHILE%d:\n", num);
        temp1 = next_reg();
        interpret_prog(stream, ((struct for_loop *)ast)->cond, temp1);
        fprintf(stream, "beq x%d, x0, ENDWHILE%d\n", temp1, num);
        free_reg();
        interpret_prog(stream, ((struct for_loop *)ast)->tl, reg);
        interpret_prog(stream, ((struct for_loop *)ast)->inc, reg);
        fprintf(stream, "jal x%d, WHILE%d\n", reg, num);
        fprintf(stream, "ENDWHILE%d:\n", num);
        break;
    }
}

void print_asm(FILE *stream, struct ast *ast)
{
    if (!ast)
    {
        return;
    }
    int stack_offset = 1;
    fprintf(stream, "jal x1, MAIN\n");
    for (int i = 0; i < NHASH; i++)
    {
        if (symtab[i].name != NULL)
        {
            fprintf(stream, "%s:\ndata 0 * 1\n", symtab[i].name);
            symtab[i].offset = stack_offset;
            stack_offset++;
        }
    }
    fprintf(stream, "MAIN:\n");
    interpret_prog(stream, ast, 1);
    fprintf(stream, "ebreak\n");
}

void print_ast(FILE *stream, struct ast *ast, int level)
{
    if (!ast)
    {
        return;
    }
    switch (ast->nodetype)
    {
    case NT_ADD:
    case NT_SUB:
    case NT_MUL:
    case NT_DIV:
        print_node(stream, level, "Operator %s\n", node2str[ast->nodetype]);
        print_ast(stream, ast->l, level + 1);
        print_ast(stream, ast->r, level + 1);
        break;
    case NT_GT:
    case NT_GTE:
    case NT_LT:
    case NT_LTE:
    case NT_EQ:
    case NT_NEQ: // comparisons
        print_node(stream, level, "Comparison %s\n", node2str[ast->nodetype]);
        print_ast(stream, ast->l, level + 1);
        print_ast(stream, ast->r, level + 1);
        break;
    case NT_LIST:
        print_node(stream, level, "List\n");
        print_ast(stream, ast->l, level + 1);
        print_ast(stream, ast->r, level + 1);
        break;
    case NT_UMINUS:
        print_node(stream, level, "Unary Minus \n");
        print_ast(stream, ast->l, level + 1);
        break;
    case NT_NUM:
        print_node(stream, level, "Constant %d\n", ((struct numval *)ast)->number);
        break;
    case NT_REF:
        print_node(stream, level, "Symbol %s\n", ((struct symref *)ast)->s->name);
        break;
    case NT_ASGN:
        print_node(stream, level, "Assignment %s\n", ((struct symasgn *)ast)->s->name);
        print_ast(stream, ((struct symasgn *)ast)->v, level + 1);
        break;
    case NT_IF:
        print_node(stream, level, "If\n");
        print_ast(stream, ((struct flow *)ast)->cond, level + 1);
        print_ast(stream, ((struct flow *)ast)->tl, level + 1);
        print_ast(stream, ((struct flow *)ast)->el, level + 1);
        break;
    case NT_WHILE:
        print_node(stream, level, "While\n");
        print_ast(stream, ((struct flow *)ast)->cond, level + 1);
        print_ast(stream, ((struct flow *)ast)->tl, level + 1);
        break;
    case NT_FOR:
        print_node(stream, level, "For\n");
        print_ast(stream, ((struct for_loop *)ast)->init, level + 1);
        print_ast(stream, ((struct for_loop *)ast)->cond, level + 1);
        print_ast(stream, ((struct for_loop *)ast)->inc, level + 1);
        print_ast(stream, ((struct for_loop *)ast)->tl, level + 1);
        break;
    case NT_AND:
    case NT_OR:
        print_node(stream, level, "Operator %s\n", node2str[ast->nodetype]);
        print_ast(stream, ast->l, level + 1);
        print_ast(stream, ast->r, level + 1);
        break;
    case NT_NOT:
        print_node(stream, level, "Operator %s\n", node2str[ast->nodetype]);
        print_ast(stream, ast->l, level + 1);
        break;
    default:
        fprintf(stream, "internal error: bad node %c\n", ast->nodetype);
    }
}

/**
 * Prints a formatted error message to the standard error stream.
 *
 * @param s The format string for the error message.
 * @param ... Additional arguments to be formatted according to the format
 * string.
 */
void yyerror(char *s, ...)
{
    va_list ap;
    va_start(ap, s);

    fprintf(stderr, "%d: error: ", yylineno);
    vfprintf(stderr, s, ap);
    fprintf(stderr, "\n");
}