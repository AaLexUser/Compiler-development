%{
#include "ast.h"
#include <stdio.h>
#include "parser.tab.h"
%}

%option noyywrap

%x COMMENT

%%

[0-9][a-zA-Z][a-zA-Z0-9]* { yyerror("Names cannot start with a digit");}
[0-9]+          { yylval.i = atoi(yytext); return NUMBER; }


    /* tokens for arithmetic and logical operators. */

    /* single character ops */
"+" |
"-" |
"*" |
"/" | 
"=" |  
"," | 
";" | 
"(" | 
")" |
"{" |
"}"           { return yytext[0]; }

    /* comparation ops, all are a CMP token */
">"                 { yylval.subtok = NT_GT; return CMP; }
"<"                 { yylval.subtok = NT_LT; return CMP; }
"!="                { yylval.subtok = NT_NEQ; return CMP; }
"=="                { yylval.subtok = NT_EQ; return CMP; }
">="                { yylval.subtok = NT_GTE; return CMP; }
"<="                { yylval.subtok = NT_LTE; return CMP; }

    /* keywords */
"if"                { return IF;     }
"else"              { return ELSE;   }
"while"             { return WHILE;  }
"for"               { return FOR;    }
"elif"              { return ELIF;   }

"&&"                { return AND;   }
"||"                { return OR;    }
"and"               { return AND;   }
"or"                { return OR;    }
"not"               { return NOT;   }
"!"                 { return NOT;   }


    /* booleans */
"true"              { yylval.i = 1; return NUMBER; }
"false"             { yylval.i = 0; return NUMBER;  }

     /* comments */

"/*"                { BEGIN(COMMENT); }
<COMMENT>"*/"       { BEGIN(INITIAL); }
<COMMENT>.          { /* consume anything inside comments */ }
<COMMENT>\n         { /* consume newlines in comments */ }
<COMMENT><<EOF>>    { yyerror("Unterminated comment"); }

"//".*              /* single line comment */

    /* names */
[a-zA-Z][a-zA-Z0-9]*    { yylval.s = strdup(yytext); return NAME; }


[ \t]+                  { /* ignore whitespace */ }
\\\n                    {printf("c> "); }  /* ignore line continuation */
\n                      {  }

.                       { printf("Unexpected character: %c\n", yytext[0]); }

%%