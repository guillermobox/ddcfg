#ifndef __AST_H_
#define __AST_H_

#include "expparser.tab.h"

struct st_resolve {
    const char * symbol;
    void * where;
};

struct st_ast_value {
    enum yytokentype type;
    union {
        int integer;
        double real;
        int boolean;
        const char * name;
    };
};

struct st_ast {
    struct st_ast * left;
    struct st_ast * right;
    char op;
    struct st_ast_value value;
};

struct st_ast *newnode(char op, struct st_ast * left, struct st_ast * right);
struct st_ast *new_constant_integer(int value);
struct st_ast *new_constant_floating(double value);
struct st_ast *new_variable(char * value);
struct st_ast_value evaluate(struct st_ast *ast);
#endif