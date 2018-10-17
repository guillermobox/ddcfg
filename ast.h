#ifndef __AST_H_
#define __AST_H_
struct st_ast {
    struct st_ast * left;
    struct st_ast * right;
    char * op;
    union {
        int ivalue;
        double dvalue;
        char *pvalue;
    };
};

struct st_ast *newnode(char op, struct st_ast * left, struct st_ast * right);
struct st_ast *new_constant_integer(int value);
struct st_ast *new_constant_floating(double value);
struct st_ast *new_variable(char * value);
double evaluate(struct st_ast *ast);
#endif