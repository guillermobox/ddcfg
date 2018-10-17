%{
  
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utilities.h"
#include "ast.h"
#include "spec.h"
extern int yylex(void);
extern int yyparse(void);

int indent=0;
String description;

struct st_spec_section * activesection;
struct st_spec_property * activeproperty;
struct st_spec * spec;

static struct st_spec_section * new_section(struct st_spec *spec)
{
	struct st_spec_section * psec;

	psec = spec->sections;

	if (psec == NULL) {
		psec = (struct st_spec_section *) malloc(sizeof(struct st_spec_section));
		spec->sections = psec;
		bzero(psec, sizeof(struct st_spec_section));
		psec->spec = spec;
		return psec;
	}

	while (psec->next != NULL)
		psec = psec->next;

	psec->next = (struct st_spec_section *) malloc(sizeof(struct st_spec_section));
	bzero(psec->next, sizeof(struct st_spec_section));
	psec->next->spec = spec;
	return psec->next;
};

static struct st_spec_property * new_property(struct st_spec_section *section)
{
	struct st_spec_property * prop;
	prop = section->properties;

	if (prop == NULL) {
		section->properties = (struct st_spec_property *) malloc(sizeof(struct st_spec_property));
		prop = section->properties;
		bzero(prop, sizeof(struct st_spec_property));
		prop->section = section;
		return prop;
	};

	while (prop->next != NULL)
		prop = prop->next;

	prop->next = (struct st_spec_property *) malloc(sizeof(struct st_spec_property));
	bzero(prop->next, sizeof(struct st_spec_property));
	prop->next->section = section;
	return prop->next;
};

struct st_ast * root = NULL;

void yyerror(const char *str)
{
    fprintf(stderr,"error: %s\n",str);
}
 
int yywrap()
{
    return 1;
} 

void print_property(struct st_spec_property *prop) {
    if (prop != NULL) {
        printf("    %s (%s)\n", prop->name, prop->description);
        print_property(prop->next);
    }
}
void print_section(struct st_spec_section *sec) {
    printf("\n\033[;1m%s (%s)\033[0m\n", sec->name, sec->description);
    print_property(sec->properties);
    if (sec->next)
        print_section(sec->next);
}
  
int main(int argc, char *argv[])
{
	spec = (struct st_spec *) malloc(sizeof(struct st_spec));
	spec->sections = NULL;

    ddcfg_parse_args(argc, argv);
    description = string_new_empty();
    yyparse();

    print_section(spec->sections);
    
    return 0;
} 

%}

%union {
    char * string;
    int integer;
    double floating;
    struct st_ast * ast;
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

section: sectionheader properties
{
}

sectionheader: newsection description
{
 	activesection->description = strdup(description->content);

    string_free(description);
    description = string_new_empty();
}

warning: newwarning description condition
{
}

failure: newfailure description condition
{
}

newfailure: FAILURE
{

}

newwarning: WARNING
{

}

condition: CONDITION expr
{
    root = $2;
}

expr: KEY
{
    $$ = new_variable($1);
}
expr: INTEGER
{
    $$ = new_constant_integer($1);
}
expr: FLOAT
{
    $$ = new_constant_floating($1);
}
expr: '(' expr ')'
{
    $$ = $2;
}
expr: expr OP expr
{
    $$ = newnode($2[0], $1, $3);
}

properties
    : property
    | property properties 

property: beginproperty options 
{
}

beginproperty: PROPERTY HALFNAME
{
    activeproperty = new_property(activesection);
    activeproperty->name = $2;
}

options
    : option
    | option options

option: TYPE TYPETOKEN
{
}

option: description
{
    activeproperty->description = strdup(description->content);
    string_free(description);
    description = string_new_empty();
}

newsection: SECTION HALFNAME
{
    activesection = new_section(spec);
    activesection->name = $2;
}

description
    : descriptionline
    | descriptionline description

descriptionline: DESCRIPTION LITERAL 
{
    string_cat_array(description, $2);
    free($2);
}
