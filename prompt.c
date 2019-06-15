#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

//Performs arithmatic functions
long evaluate_operation(long num, char *operator, long num2) {
    if (strcmp(operator, "+") == 0) return num + num2;
    if (strcmp(operator, "-") == 0) return num - num2;
    if (strcmp(operator, "*") == 0) return num * num2;
    if (strcmp(operator, "/") == 0) return num / num2;
    if (strcmp(operator, "%") == 0) return num % num2;
    if (strcmp(operator, "^") == 0) return pow(num, num2);
}

long negate(long num) {
    return num * -1;
}

long evaluate(mpc_ast_t* tree) {

    //Base Case
    if(strstr(tree->tag, "number")) {
        return atoi(tree->contents);
    }

    //Gets the operator from subprocedure
    char* operator = tree->children[1]->contents;

    //Gets the first number used in subprocedure
    long num = evaluate(tree->children[2]);

    //Go through the children of the tree and perform operations on returned value
    int i = 3;
    if (strstr(tree->children[i]->tag, "expression")) {
        do {
        num = evaluate_operation(num, operator, evaluate(tree->children[i]));
        i++;
        }
        while (strstr(tree->children[i]->tag, "expression"));
    }
    else if (strcmp(operator, "-") == 0) num = negate(num);

    return num;
}

int main(int argc, char** argv) {
    
    //Notation

    mpc_parser_t* Number = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expression = mpc_new("expression");
    mpc_parser_t* Crisp = mpc_new("crisp");

    mpca_lang(MPCA_LANG_DEFAULT,
    "                        \
        number : /-?[0.0-9]+/; \
        operator : '+' | '-' | '/' | '*' | '%' | '^'; \
        expression : <number> | '(' <operator> <expression>+ ')'; \
        crisp: /^/ <operator> <expression>+ /$/; \
    ", Number, Operator, Expression, Crisp);


    //Information
    puts("Crisp v0.1.0");
    puts("Press Ctrl+c to exit \n");

    //Main loop
    int debug = 0;
    while (1) {
        char* input =readline("crisp> ");
        add_history(input);
        //Attempt to parse
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Crisp, &r) && debug == 0) {
            long result = evaluate(r.output);
            printf("%li\n", result);
            mpc_ast_delete(r.output);
        }
        else if (mpc_parse("<stdin>", input, Crisp, &r) && debug == 1) {
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