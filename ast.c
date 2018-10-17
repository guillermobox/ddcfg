#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ddcfg.h"

struct st_ast {
    struct st_ast * left;
    struct st_ast * right;
    char op;
    union {
        int    ivalue;
        double dvalue;
        char  *pvalue;
    };
};

void ast_free(struct st_ast * ast) {
    if (ast->left) {
        ast_free(ast->left);
        ast->left = NULL;
    }
    if (ast->right) {
        ast_free(ast->right);
        ast->right = NULL;
    }
    free(ast);
}

struct st_ast *newnode(char op, struct st_ast * left, struct st_ast * right) {
    struct st_ast * n;
    n = (struct st_ast *) malloc(sizeof(*n));
    n->left = left;
    n->right = right;
    n->op = op;
    return n;
}

struct st_ast *new_constant_integer(int value) {
    struct st_ast * node = newnode('i', NULL, NULL);
    node->ivalue = value;
    return node;
}

struct st_ast *new_constant_floating(double value) {
    struct st_ast * node = newnode('f', NULL, NULL);
    node->dvalue = value;
    return node;
}

struct st_ast *new_variable(char * value) {
    struct st_ast * node = newnode('v', NULL, NULL);
    node->pvalue = value;
    return node;
}

double evaluate(struct st_ast *ast) {
    double x, y;
    char *section, *option;
    switch(ast->op) {
        case '+':
            x = evaluate(ast->left);
            y = evaluate(ast->right);
            return x + y;
        case '*':
            x = evaluate(ast->left);
            y = evaluate(ast->right);
            return x * y;
        case '&':
            x = evaluate(ast->left);
            y = evaluate(ast->right);
            return x && y;
        case '|':
            x = evaluate(ast->left);
            y = evaluate(ast->right);
            return x || y;
        case 'i':
            return (double) ast->ivalue;
        case 'f':
            return ast->dvalue;
        case 'v':
            section = ast->pvalue;
            option = index(section, '.');
            *option = '\0';
            option = option + 1;
            return ddcfg_double(section, option);
    }
    return 0.0;
}