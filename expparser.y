%{
  
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utilities.h"
#include "ast.h"
#include "spec.h"
extern int yylex(void);
extern int yyparse(void);
#define YYERROR_VERBOSE
int indent=0;
String description;
Stack st;

struct st_spec_section * activesection;
struct st_spec_property * activeproperty;
struct st_spec_constraint * activecons;
struct st_spec * spec;

void queue_symbol(const char *name, void * where) {
    struct st_resolve * symbol = malloc(sizeof(*symbol));
    symbol->symbol = name;
    symbol->where = where;
    stack_push(st, symbol);
};
struct st_spec_property * property_from_string(const char * s);

void resolve_symbols() {
    void * p = stack_pop(st);
    while (p) {
        struct st_spec_property * prop;
        struct st_resolve * sym = (struct st_resolve *) p;
        prop = property_from_string(sym->symbol);
        if (prop) {
            *((struct st_spec_property **) sym->where) = prop;
        } else {
            printf("Symbol impossible to resolve!\n");
        }
        free(p);
        p = stack_pop(st);
    }
};

struct st_spec_property * property_from_string(const char * s)
{
    struct st_spec_section * sec;
    struct st_spec_property *prop;
    const char * secname, *propname;
    char * p;
    size_t len;

    p = strchr(s, '.');
    len = p - s;
    secname = s;
    sec = spec->sections;
    while (sec) {
        if (strncmp(sec->name, secname, len) == 0)
            break;
        sec = sec->next;
    }
    if (sec == NULL) return NULL;
    p = p + 1;
    len = strlen(s) - 1 - len;
    prop = sec->properties;
    while (prop) {
        if (strncmp(prop->name, p, len) == 0)
            break;
        prop = prop->next;
    }
    return prop;
};

static struct st_spec_constraint * new_constraint(struct st_spec *spec)
{
    struct st_spec_constraint * cons;

    cons = spec->constraints;

    if (cons == NULL) {
        cons = (struct st_spec_constraint *) malloc(sizeof(*cons));
        spec->constraints = cons;
        bzero(cons, sizeof(*cons));
        cons->spec = spec;
        return cons;
    }

    while (cons->next != NULL)
        cons = cons->next;

    cons->next = (struct st_spec_constraint *) malloc(sizeof(*cons));
    bzero(cons->next, sizeof(*cons));
    cons->next->spec = spec;
    return cons->next;    
};

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
        prop->type = T_TYPE_STRING;
		return prop;
	};

	while (prop->next != NULL)
		prop = prop->next;

	prop->next = (struct st_spec_property *) malloc(sizeof(struct st_spec_property));
	bzero(prop->next, sizeof(struct st_spec_property));
	prop->next->section = section;
    prop->next->type = T_TYPE_STRING;
	return prop->next;
};

struct st_ast * root = NULL;
const char * root_expression;
extern int yylineno;

void yyerror(const char *str)
{
    fprintf(stderr,"Error | Line: %d %s\n", yylineno, str);
};
 
int yywrap()
{
    return 1;
} 

void print_property(struct st_spec_property *prop)
{
    if (prop != NULL) {
        const char * type;
        if (prop->type == T_TYPE_INTEGER) {
            type = "integer";
        } else if (prop->type == T_TYPE_REAL) {
            type = "real";
        } else if (prop->type == T_TYPE_BOOLEAN) {
            type = "boolean";
        } else if (prop->type == T_TYPE_STRING) {
            type = "string";
        } 

        printf("    %s <%s> (%s)\n", prop->name, type, prop->description);
        if (prop->depends_on) {
            printf("        depending on this: %s.%s\n",
                prop->depends_on->section->name,
                prop->depends_on->name);
        }

        print_property(prop->next);
    }
}

void print_section(struct st_spec_section *sec)
{

    printf("\n\033[;1mSection: %s\033[0m (%s)\n", sec->name, sec->description);
    print_property(sec->properties);

    if (sec->next)
        print_section(sec->next);
}

void print_constraint(struct st_spec_constraint *cons)
{
    const char * type;
    if (cons == NULL) {
        printf("\nEND!\n");
        return;
    }
    if (cons->type == T_WARNING) type = "warning";
    else if (cons->type == T_FAILURE) type = "failure";
    printf("\n\033[;1mConstraint: %s\033[0m (%s)\n", type, cons->description);
    printf("     ast: %s\n", cons->expression);
    print_constraint(cons->next);
}

void structure_property(struct st_spec_property *prop) 
{
        if (prop != NULL) {
        const char * type;
        if (prop->type == T_TYPE_INTEGER) {
            type = "int";
        } else if (prop->type == T_TYPE_REAL) {
            type = "double";
        } else if (prop->type == T_TYPE_BOOLEAN) {
            type = "int";
        } else if (prop->type == T_TYPE_STRING) {
            type = "char* ";
        } 
        printf("        %s %s; /*!< %s */\n", type, prop->name, prop->description);
        structure_property(prop->next);
    }
}

void structure_section(struct st_spec_section *sec)
{
    printf("\nstruct ddcfg_database {\n");
    while (sec) {
        printf("    struct {\n");
        structure_property(sec->properties);
        printf("    } %s; /*!< %s */\n", sec->name, sec->description);
        sec = sec->next;
    }
    printf("} database;\n\n");
}

void load_structure_property(struct st_spec_property *prop)
{
    const char *sec, *pro, *command;
    sec = prop->section->name;
    pro = prop->name;
    switch(prop->type) {
        case T_TYPE_INTEGER: command = "ddcfg_int"; break;
        case T_TYPE_REAL:    command = "ddcfg_double"; break;
        case T_TYPE_BOOLEAN: command = "ddcfg_bool"; break;
        case T_TYPE_STRING:  command = "ddcfg_get"; break;
        default: break;
    }
    printf("    database.%s.%s = %s(\"%s\", \"%s\");\n", sec, pro, command, sec, pro);
}

void load_structure_section(struct st_spec_section *sec)
{
    struct st_spec_property *prop = sec->properties;
    while (prop) {
        load_structure_property(prop);
        prop = prop->next;
    }
}

void load_structures(struct st_spec * spec)
{
    struct st_spec_section * sec = spec->sections;

    printf("void freeze_database()\n{\n");
    while (sec) {
        load_structure_section(sec);
        sec = sec->next;
    }
    printf("}\n");
}

int main(int argc, char *argv[])
{
	spec = (struct st_spec *) malloc(sizeof(struct st_spec));
	spec->sections = NULL;
    st = stack_new(0);

    ddcfg_parse_args(argc, argv);
    description = string_new_empty();
    yyparse();

    resolve_symbols();


    print_section(spec->sections);
    print_constraint(spec->constraints);

    return 0;
} 

%}
%union {
    char * string;
    int integer;
    double floating;
    struct st_ast * ast;
    enum yytokentype token;
}

%token T_SECTION T_SECMARKER T_DESCRIPTION T_PROPERTY T_TYPE
%token T_FAILURE T_WARNING T_CONDITION T_DEPENDS_ON T_POINTS_TO
%token <token> T_TYPETOKEN T_TYPE_INTEGER T_TYPE_REAL T_TYPE_BOOLEAN T_TYPE_STRING T_TYPE_SUBSECTION
%token <string> T_HALFNAME T_FULLNAME T_LITERAL T_KEY T_OP
%token <integer> T_INTEGER
%token <floating> T_FLOAT
%type <ast> expr
%expect 2


%%
spec: /* empty */
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
    activecons->description = strdup(description->content);
    string_free(description);
    description = string_new_empty();
    activecons->ast = root;
    activecons->expression = root_expression;
}

failure: newfailure description condition
{
    activecons->description = strdup(description->content);
    string_free(description);
    description = string_new_empty();
    activecons->ast = root;
    activecons->expression = root_expression;
}

newfailure: T_FAILURE
{
    activecons = new_constraint(spec);
    activecons->type = T_FAILURE;
}

newwarning: T_WARNING
{   
    activecons = new_constraint(spec);
    activecons->type = T_WARNING;
}

condition: T_CONDITION T_LITERAL expr
{
    root = $3;
    root_expression = $2;
}

expr: T_KEY
{
    $$ = new_variable($1);
}
expr: T_INTEGER
{
    $$ = new_constant_integer($1);
}
expr: T_FLOAT
{
    $$ = new_constant_floating($1);
}
expr: '(' expr ')'
{
    $$ = $2;
}
expr: expr T_OP expr
{
    $$ = newnode($2[0], $1, $3);
}

properties
    : property
    | property properties 

property: beginproperty options 
{
}

beginproperty: T_PROPERTY T_HALFNAME
{
    activeproperty = new_property(activesection);
    activeproperty->name = $2;
}

options
    : option
    | option options

option: T_TYPE T_TYPETOKEN
{
    activeproperty->type = $2;
}
option: T_DEPENDS_ON T_FULLNAME
{
    queue_symbol($2, &activeproperty->depends_on);
}
option: T_POINTS_TO T_FULLNAME
{
    queue_symbol($2, &activeproperty->points_to);
}
option: description
{
    activeproperty->description = strdup(description->content);
    string_free(description);
    description = string_new_empty();
}

newsection: T_SECTION T_HALFNAME
{
    activesection = new_section(spec);
    activesection->name = $2;
}

description
    : descriptionline
    | descriptionline description

descriptionline: T_DESCRIPTION T_LITERAL 
{
    string_cat_array(description, $2);
    free($2);
}
