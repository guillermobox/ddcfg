%{

    
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utilities.h"

extern int yylex(void);
extern int yyparse(void);

int indent=0;
String description;

struct st_node {
    struct st_node * left;
    struct st_node * right;
    char * op;
    union {
        int ivalue;
        double dvalue;
        char *pvalue;
    };
};

struct st_node *newnode(char * op, struct st_node * left, struct st_node * right) {
    struct st_node * n;
    n = malloc(sizeof(*n));
    n->left = left;
    n->right = right;
    n->op = strdup(op);
    return n;
}

struct st_node *new_constant_integer(int value) {
    struct st_node * n;
    n = malloc(sizeof(*n));
    n->left = NULL;
    n->right = NULL;
    n->op = "integer";
    n->ivalue = value;
    return n;
}

struct st_node *new_constant_floating(double value) {
    struct st_node * n;
    n = malloc(sizeof(*n));
    n->left = NULL;
    n->right = NULL;
    n->op = "floating";
    n->dvalue = value;
    return n;
}

struct st_node *newfinal(char * value) {
    struct st_node * n;
    n = malloc(sizeof(*n));
    n->left = NULL;
    n->right = NULL;
    n->op = strdup(value);
    return n;
}

struct st_node * root = NULL;

double evaluate(struct st_node *ast) {
    double x, y;
    if (strcmp(ast->op, "integer") == 0) {
        return (double) ast->ivalue;
    }
    if (strcmp(ast->op, "floating") == 0) {
        return ast->dvalue;
    }
    if (strcmp(ast->op, "+") == 0) {
        x = evaluate(ast->left);
        y = evaluate(ast->right);
        return x + y;
    }
    if (strcmp(ast->op, "*") == 0) {
        x = evaluate(ast->left);
        y = evaluate(ast->right);
        return x * y;
    }
}

void print(char *msg) {
    int i;
    for (i=indent; i>0; i--) 
        printf("  ");
    printf("%s\n", msg);
}

void more(){
    indent = indent + 1;
}

void less() {
    indent = indent - 1;
}

void yyerror(const char *str)
{
    fprintf(stderr,"error: %s\n",str);
}
 
int yywrap()
{
    return 1;
} 
  
int main()
{
    description = string_new_empty();
    yyparse();
    return 0;
} 

%}

%union {
    char * string;
    int integer;
    double floating;
    struct st_node * ast;
}

%token SECTION SECMARKER DESCRIPTION PROPERTY TYPE
%token FAILURE WARNING CONDITION
%token <string> HALFNAME TYPETOKEN LITERAL KEY OP
%token <integer> INTEGER
%token <floating> FLOAT
%type <ast> expr

%%
spec:
    | section spec 
    | failure spec
    | warning spec

section: sectionheader properties {
    less();
}

sectionheader: newsection description {
        printf("description: <%s>\n", description->content);
        string_free(description);
            description = string_new_empty();
    }

warning: newwarning description condition {
    less();
}

failure: newfailure description condition {
    less();
}

newfailure: FAILURE {
    printf("Failure...\n");
    more();
}

newwarning: WARNING {
    printf("Warning...\n");
    more();
}

condition: CONDITION expr {
    print("Condition was parsed fine");
    root = $2;
}

expr
    : KEY {$$ = newfinal($1);}
    | INTEGER {$$ = new_constant_integer($1);}
    | FLOAT {$$ = new_constant_floating($1);}
    | '(' expr ')' {$$ = $2;}
    | expr OP expr {$$ = newnode($2, $1, $3);}

properties
    : property
    | property properties 

property
    : beginproperty options {
        less();
    }

beginproperty: PROPERTY HALFNAME {
        print($2);
        more();
    }

options
    : option
    | option options

option
    : TYPE TYPETOKEN {
        print($2);
    }
    | description {
        printf("description: <%s>\n", description->content);
        string_free(description);
            description = string_new_empty();
    }

newsection
    : SECTION HALFNAME {
        print($2);
        more();
    }

description
    : descriptionline
    | descriptionline description

descriptionline
    : DESCRIPTION LITERAL {
        string_cat_array(description, $2);
        free($2);
    }
