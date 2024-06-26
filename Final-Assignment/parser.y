%{
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
void yyerror(char *s, ...);
int yylex(void);
%}

%union {
    struct ast *a;
    int i;
    char *s; /* which symbol */
    ntype_t subtok;
}

/* declare tokens */
%token <i> NUMBER
%token <s> NAME
%token EOL YYEOF
%token IF ELSE WHILE FOR ELIF

%right '='
%left OR
%left AND
%left NOT
%left <subtok> CMP
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS
%type <a> exp list flow if_stmt elif_stmt else_stmt

%start prog
%%
prog: list YYEOF                     {
                                            if($1 != NULL) {
                                                print_ast(stdout, $1, 0);
                                                FILE* fl = fopen("out.S", "w");
                                                FILE* fl2 = fopen("tree", "w");
                                                if(fl == NULL) {
                                                    printf("Error opening file\n");
                                                    exit(1);
                                                }
                                                if(fl2 == NULL) {
                                                    printf("Error opening file\n");
                                                    exit(1);
                                                }
                                                print_asm(fl, $1);
                                                print_ast(fl2, $1, 0);
                                                fclose(fl);
                                                treefree($1);
                                            }
                                            printf("> ");
                                        }
    | error YYEOF                    { yyerrok; printf("> ");}
    ;

flow: if_stmt
    | WHILE exp '{' list '}'                        { $$ = newflow(NT_WHILE, $2, $4, NULL);  }
    | FOR '(' exp ';' exp ';' exp ')' '{' list '}'  { $$ = newfor($3, $5, $7, $10); }
    ;

if_stmt: IF exp '{' list '}' else_stmt              { $$ = newflow(NT_IF, $2, $4, $6); }
    | IF exp '{' list '}' elif_stmt                 { $$ = newflow(NT_IF, $2, $4, $6); }
    ;
elif_stmt: ELIF exp '{' list '}' else_stmt          { $$ = newflow(NT_IF, $2, $4, $6); }
    | ELIF exp '{' list '}' elif_stmt               { $$ = newflow(NT_IF, $2, $4, $6); }
    ;
else_stmt:  /* nothing */                           { $$ = NULL; }
    | ELSE '{' list '}'                             { $$ = $3; }
    ;
list: /* nothing */                     { $$ = NULL; }
    | exp ';' list                      {
                                            if($3 == NULL) $$ = $1;
                                            else $$ = newast(NT_LIST, $1, $3);
                                        }
    | flow list                         {
                                            if($2 == NULL) $$ = $1;
                                            else $$ = newast(NT_LIST, $1, $2);
                                        }
    ;

exp:  exp CMP exp                       { $$ = newcmp($2, $1, $3);      }                       
    | exp '+' exp                       { $$ = newast(NT_ADD, $1, $3);     }
    | exp '-' exp                       { $$ = newast(NT_SUB, $1, $3);     }
    | exp '*' exp                       { $$ = newast(NT_MUL, $1, $3);     }
    | exp '/' exp                       { $$ = newast(NT_DIV, $1, $3);     }
    | '(' exp ')'                       { $$ = $2;                      }
    | '-' exp  %prec UMINUS             { $$ = newast(NT_UMINUS, $2, NULL);   }
    | exp AND exp                       { $$ = newast(NT_AND, $1, $3);   }
    | exp OR exp                        { $$ = newast(NT_OR, $1, $3);    }
    | NOT exp                           { $$ = newast(NT_NOT, $2, NULL); }
    | NUMBER                            { $$ = newnum($1);              }
    | NAME                              { $$ = newref($1);              }
    | NAME '=' exp                      { $$ = newasgn($1, $3);         }
    ;
%%



