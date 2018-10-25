#ifndef __AST_H_
#define __AST_H_

#include "expparser.tab.h"

struct st_resolve {
    const char * symbol;
    void * where;
    enum yytokentype type;
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
    enum yytokentype op;
    struct st_ast_value value;
};

/* Create a general node */
struct st_ast *newnode(enum yytokentype op, struct st_ast * left, struct st_ast * right);

/* Create a leaf node */
struct st_ast *new_constant_integer(int);
struct st_ast *new_constant_real(double);
struct st_ast *new_constant_boolean(int);
struct st_ast *new_variable(char *);

/* Evaluate the AST */
struct st_ast_value evaluate(struct st_ast *ast);
#endif