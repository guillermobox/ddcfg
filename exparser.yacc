%{
#include <stdio.h>
#include <string.h>
 
int indent=0;

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
  
main()
{
    yyparse();
} 

%}

%token LITERAL SECTION SECMARKER DESCRIPTION PROPERTY KEY HALFNAME TYPE TYPETOKEN
%token FAILURE WARNING CONDITION INTEGER FLOAT OP

%%
spec:
    | section spec 
    | failure spec
    | warning spec

section: newsection description properties {
    less();
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
}

expr
    : KEY
    | INTEGER
    | FLOAT
    | '(' expr ')'
    | expr OP expr

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
    | description

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
        print($2);
    }
