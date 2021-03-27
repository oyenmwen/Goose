#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

char* LANG = "goose"; //dont forget to change the value
                      // in mpca_lang as well
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt){
    fputs(prompt,stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+ 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\0';
    return cpy;
}

void add_history(*char unused){}

#else

#include <editline/readline.h>

#endif


int main() {
    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Decimal = mpc_new("decimal");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* Lang = mpc_new(LANG);

    mpca_lang(MPCA_LANG_DEFAULT,
              "                     \
          number  : /-?[0-9]+/;     \
          decimal : <number>+ '.' <number>+; \
          operator : '+' | '-' | '*' | '/'; \
          expr : <operator> | <number> | <decimal> | '(' <decimal> <expr>+ ')' | '(' <number> <expr>+ ')'; \
          goose : /^/ '('? <decimal> <expr>+ ')'? | '('? <number> <expr>+ ')'? /$/ ;   \
          ",
              Number, Decimal, Operator, Expr, Lang);

    printf("%s Version 0.0.0.1", LANG);
    puts("MIT License");
    puts("Press CTR+C to Exit\n");

    while (1) {
        char tag[sizeof(LANG)];
        strcpy(tag, LANG);
        strcat(tag,"> ");
        char *input = readline(tag);
        add_history(input);
        mpc_result_t r;
        if(mpc_parse("<stdin",input,Lang, &r)){
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        }else{
            mpc_err_print(r.error);
            mpc_err_print(r.error);
        }
        free(input);
    }

    mpc_cleanup(4, Number, Operator, Expr, Lang);
    return 0;
}
