#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

//Windows functions
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline (char* prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char* cpy = malloc(strlen(buffer)+1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy)-1] = '\O';
    return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#endif

int main(int argc, char** argv) {
    
    //Notation

    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expression = mpc_new("expression");
    mpc_parser_t* Crisp = mpc_new("crisp");

    mpca_lang(MPCA_LANG_DEFAULT,
    "                        \
        number : /-?[0.0-9]+/; \
        operator : '+' | '-' | '/' | '*' | '%'; \
        expression : <number> | '(' <operator> <expression>+ ')'; \
        crisp: /^/ <operator> <expression>+ /$/; \
    ", Number, Operator, Expression, Crisp);


    //Information
    puts("Crisp v0.0.2");
    puts("Press Ctrl+c to exit \n");

    //Main loop
    while (1) {
        char* input =readline("crisp> ");
        add_history(input);
        //Attempt to parse
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Crisp, &r)) {
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        }
        else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(input);
    }

    //Cleanup
    mpc_cleanup(4, Number, Operator, Expression, Crisp);
    return 0;
}

long evaluation(mpc_ast_t* tree) {

    //Base Case

}