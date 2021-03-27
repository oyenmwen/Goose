#include <stdio.h>
#include <stdlib.h>

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
    puts("Goose Version 0.0.0.1");
    puts("MIT License");
    puts("Press CTR+C to Exit\n");

    while(1){

        char* input = readline("goose> ");
        add_history(input);

        printf("What does the goose say? '%s'", input);
        free(input);
    }
    return 0;
}
