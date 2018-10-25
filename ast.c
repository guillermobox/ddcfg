#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ddcfg.h"

#include "ast.h"
#include "spec.h"


void ast_free(struct st_ast * ast)
{
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

struct st_ast *newnode(enum yytokentype op, struct st_ast * left, struct st_ast * right)
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
    struct st_ast * node = newnode(T_INTEGER, NULL, NULL);
    node->value.type = T_TYPE_INTEGER;
    node->value.integer = value;
    return node;
}

struct st_ast *new_constant_real(double value)
{
    struct st_ast * node = newnode(T_REAL, NULL, NULL);
    node->value.type = T_TYPE_REAL;
    node->value.real = value;
    return node;
}

struct st_ast *new_constant_boolean(int value)
{
    struct st_ast *node = newnode(T_BOOLEAN, NULL, NULL);
    node->value.type = T_TYPE_BOOLEAN;
    node->value.boolean = value;
    return node;
}

struct st_ast *new_variable(char * value)
{
    struct st_ast * node = newnode(T_FULLNAME, NULL, NULL);
    node->value.type = T_FULLNAME;
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
        case T_INTEGER:
        case T_REAL:
            return ast->value;
        case T_FULLNAME:
            result = fetch_symbol(ast->value.name);
            return result;
        case T_NOT:
            result.boolean = !ast->left->value.boolean;
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
            case T_ADD:
                result.real = left.real + right.real;
                return result;
            case T_MULTIPLY:
                result.real = left.real * right.real;
                return result;
            case T_GREATER:
                result.boolean = left.real > right.real;
                result.type = T_TYPE_BOOLEAN;
                return result;
            case T_LESS:
                result.boolean = left.real < right.real;
                result.type = T_TYPE_BOOLEAN;
                return result;
            default:
                printf("Unknown!\n");
        }
    } else if(result.type == T_TYPE_INTEGER) {
        switch(ast->op) {
            case T_ADD:
                result.integer = left.integer + right.integer;
                return result;
            case T_MULTIPLY:
                result.integer = left.integer * right.integer;
                return result;
            case T_GREATER:
                result.boolean = left.integer > right.integer;
                result.type = T_TYPE_BOOLEAN;
                return result;
            case T_LESS:
                result.boolean = left.integer < right.integer;
                result.type = T_TYPE_BOOLEAN;
                return result;
            default:
                printf("Unknown!\n");
        }
    } else if (result.type == T_TYPE_BOOLEAN) {
        switch (ast->op) {
            case T_AND:
                result.boolean = left.boolean && right.boolean;
                return result;
            case T_OR:
                result.boolean = left.boolean || right.boolean;
                return result;
            default:
                printf("Unknown for boolean operation\n");
        }
    }

    return result;
}


void ast_print(struct st_ast * ast)
{
    printf("\"%p\" ", ast);
    switch (ast->op) {
        case T_FULLNAME:
            printf("[shape=trapezium, label=\"%s\"]\n", ast->value.name);
            break;
        case T_AND:
            printf("[fillcolor=darkslategray3, shape=box, label=AND]\n");
            break;
        case T_OR:
            printf("[fillcolor=darkslategray3, shape=box, label=OR]\n");
            break;
        case T_ADD:
            printf("[fillcolor=darkolivegreen3, shape=box, label=\"+\"]\n");
            break;
        case T_MULTIPLY:
            printf("[fillcolor=darkolivegreen3, shape=box, label=\"&times;\"]\n");
            break;
        case T_INTEGER:
            printf("[shape=oval, label=\"%d\"]\n", ast->value.integer);
            break;
        case T_REAL:
            printf("[shape=oval, label=\"%f\"]\n", ast->value.real);
            break;
        case T_GREATER:
            printf("[fillcolor=darkslategray3, shape=box, label=\">\"]\n");
            break;
        case T_LESS:
            printf("[fillcolor=darkslategray3, shape=box, label=\"<\"]\n");
            break;
        default:
            printf("\n");
    }

    if (ast->left && ast->right) {
        printf("\"%p\" -> {\"%p\" \"%p\"}\n", ast, ast->left, ast->right);
        ast_print(ast->left);
        ast_print(ast->right);
    }
}

void ast_export_to_dot(struct st_ast * ast)
{
    printf("digraph {\n");
    printf(" node [fontname = \"helvetica\"];\n");

    ast_print(ast);

    printf("}\n");
}