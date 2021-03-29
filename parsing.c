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

typedef struct{
    int type;
    long num;
    int err;
}lval;

enum { LVAL_NUM, LVAL_ERR};

enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM};

lval lval_num(long x){
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

lval lval_err(int x){
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

void lval_print(lval v){
    switch (v.type){

        case LVAL_NUM:
            printf("%li", v.num);
            break;

        case LVAL_ERR:
            if (v.err == LERR_DIV_ZERO){
                printf("Error: Division By Zero !");
            }
            if (v.err == LERR_BAD_OP){
                printf("Error: Invalid Operator!");
            }
            if (v.err == LERR_BAD_NUM){
                printf("Error: Invalid Number!");
            }
            break;
    }
}

void lval_println(lval v) {
    lval_print(v);
    putchar('\n');
}

lval eval_op(lval x, char *op, lval y) {

    if (x.type == LVAL_ERR){
        return x;
    }

    if (y.type == LVAL_ERR){
        return y;
    }

    if(strcmp(op, "+") == 0) {
        return lval_num(x.num + y.num);
    }
    if(strcmp(op, "-") == 0) {
        return lval_num(x.num - y.num);
    }
    if(strcmp(op, "*") == 0) {
        return lval_num(x.num * y.num);
    }
    if(strcmp(op, "/") == 0) {
        return y.num == 0
               ? lval_err(LERR_DIV_ZERO)
               : lval_num(x.num / y.num);
    }
    return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t){

    if(strstr(t->tag, "number")){
        errno = 0;
        long x = strtol(t->contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }
    char* op = t->children[1]->contents;

    lval x = eval(t->children[2]);


    int i = 3;

    while(strstr(t->children[i]->tag,"expr")){
        x = eval_op(x, op, eval(t->children[i]));
        i++;
    }

    return x;
}


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
          expr :  <number> | <decimal>  | '(' <operator> <expr>+ ')'; \
          goose : /^/ '('? <operator> <expr>+ ')'?  /$/ ;   \
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
            lval result = eval(r.output);
            lval_println(result);
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
