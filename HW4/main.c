#include <stdio.h>
extern int yyparse(void);
#ifdef YYDEBUG
    extern int yydebug;
#endif
int main()
{
    #ifdef YYDEBUG
        yydebug = 1;
    #endif
    printf("> ");
    return yyparse();
}