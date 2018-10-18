#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ddcfg.h"

#include "ast.h"
#include "spec.h"

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

struct st_ast *newnode(char op, struct st_ast * left, struct st_ast * right)
{
    struct st_ast * n;
    n = (struct st_ast *) malloc(sizeof(*n));
    n->left = left;
    n->right = right;
    n->op = op;
    return n;
}

struct st_ast *new_constant_integer(int value)
{
    struct st_ast * node = newnode('i', NULL, NULL);
    node->value.type = T_TYPE_INTEGER;
    node->value.integer = value;
    return node;
}

struct st_ast *new_constant_floating(double value)
{
    struct st_ast * node = newnode('f', NULL, NULL);
    node->value.type = T_TYPE_REAL;
    node->value.real = value;
    return node;
}

struct st_ast *new_variable(char * value)
{
    struct st_ast * node = newnode('v', NULL, NULL);
    node->value.type = T_KEY;
    node->value.name = value;
    return node;
}

struct st_spec_property * property_from_string(const char * s);

struct st_ast_value fetch_symbol(const char *fullname)
{
    struct st_ast_value val;
    struct st_spec_property * prop = property_from_string(fullname);

    val.type = prop->type;

    switch(val.type) {
        case T_TYPE_REAL:
            val.real = ddcfg_double(NULL, fullname); break;
        case T_TYPE_INTEGER:
            val.integer = ddcfg_int(NULL, fullname); break;
        case T_TYPE_BOOLEAN:
            val.boolean = ddcfg_bool(NULL, fullname); break;
        default: break;
    }

    return val;
}

enum yytokentype getcommontype(struct st_ast_value x, struct st_ast_value y)
{
    if (x.type == T_TYPE_BOOLEAN && y.type == T_TYPE_BOOLEAN)
        return T_TYPE_BOOLEAN;
    if ( (x.type == T_TYPE_REAL && y.type == T_TYPE_INTEGER) ||
         (x.type == T_TYPE_INTEGER && y.type == T_TYPE_REAL) ||
         (x.type == T_TYPE_REAL && y.type == T_TYPE_REAL))
        return T_TYPE_REAL;
    if (x.type == T_TYPE_INTEGER && y.type == T_TYPE_INTEGER)
        return T_TYPE_INTEGER;
    printf("ERROR!");
};

void cast(struct st_ast_value *val, enum yytokentype type)
{
    if (val->type == type)
        return;
    else if (val->type == T_TYPE_INTEGER && type == T_TYPE_REAL) {
        val->real = (double) val->integer;
        val->type = T_TYPE_REAL;
        return;
    }
    printf("Impossible to cast!\n");
};

struct st_ast_value evaluate(struct st_ast *ast)
{
    char *section, *option;
    struct st_ast_value left, right, result;

    /* this is a read operation */
    switch(ast->op) {
        case 'i':
        case 'f':
            return ast->value;
        case 'v':
            result = fetch_symbol(ast->value.name);
            return result;
        default:
            break;
    }

    /* this is a binary operation */
    left = evaluate(ast->left);
    right = evaluate(ast->right);
    result.type = getcommontype(left, right);

    cast(&left, result.type);
    cast(&right, result.type);

    if (result.type == T_TYPE_REAL) {
        switch(ast->op) {
            case '+':
                result.real = left.real + right.real;
                return result;
            case '*':
                result.real = left.real * right.real;
                return result;
            case '>':
                result.boolean = left.real > right.real;
                result.type = T_TYPE_BOOLEAN;
                return result;
            case '<':
                result.boolean = left.real < right.real;
                result.type = T_TYPE_BOOLEAN;
                return result;
            default:
                printf("Unknown!\n");
        }
    } else if(result.type == T_TYPE_INTEGER) {
        switch(ast->op) {
            case '+':
                result.integer = left.integer + right.integer;
                return result;
            case '*':
                result.integer = left.integer * right.integer;
                return result;
            case '>':
                result.boolean = left.integer > right.integer;
                result.type = T_TYPE_BOOLEAN;
                return result;
            case '<':
                result.boolean = left.integer < right.integer;
                result.type = T_TYPE_BOOLEAN;
                return result;
            default:
                printf("Unknown!\n");
        }
    } else if (result.type == T_TYPE_BOOLEAN) {
        switch (ast->op) {
            case 'A':
                result.boolean = left.boolean && right.boolean;
                return result;
            case 'O':
                result.boolean = left.boolean || right.boolean;
                return result;
            default:
                printf("Unknown for boolean operation\n");
        }
    }

    return result;
}
