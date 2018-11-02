%{
  
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utilities.h"
#include "ast.h"
#include "spec.h"

extern struct st_spec * spec;
extern int yylex(void);
extern int yyparse(void);
#define YYERROR_VERBOSE
int indent=0;
String description;
Stack st;

struct st_spec_section * activesection;
struct st_spec_property * activeproperty;
struct st_spec_constraint * activecons;


void queue_symbol(const char *name, void * where, enum yytokentype type) {
    struct st_resolve * symbol = malloc(sizeof(*symbol));
    symbol->symbol = name;
    symbol->where = where;
    symbol->type = type;
    stack_push(st, symbol);
};

struct st_spec_property * property_from_string(const char * s);
struct st_spec_section * section_from_string(const char * s);

void resolve_symbols() {
    void * p = stack_pop(st);
    while (p) {
        struct st_spec_property * prop;
        struct st_spec_section * sec;
        struct st_resolve * sym = (struct st_resolve *) p;

        if (sym->type == T_PROPERTY) {
            prop = property_from_string(sym->symbol);
            if (prop) {
                *((struct st_spec_property **) sym->where) = prop;
            } else {
                //printf("Symbol impossible to resolve! %s\n", sym->symbol);
            }
        } else if (sym->type == T_SECTION) {
            sec = section_from_string(sym->symbol);
            if (sec) {
                *((struct st_spec_section **) sym->where) = sec;
            } else {
                //printf("Symbol for section impossible to resolve! %s\n", sym->symbol);
            }
        }
        free(p);
        p = stack_pop(st);
    }
};

struct st_spec_section * section_from_string(const char * s)
{
    struct st_spec_section * sec;
    size_t len;
    len = strlen(s);
    sec = spec->sections;
    while (sec) {
        if (strncmp(sec->name, s, len) == 0)
            break;
        sec = sec->next;
    }
    return sec;
}

struct st_spec_property * property_from_string(const char * s)
{
    struct st_spec_section * sec;
    struct st_spec_property *prop;
    const char * secname;
    char * p;
    size_t len;

    p = strchr(s, '.');
    if (p == NULL) {
        printf("This string is not a property!\n");
        return 0;
    }
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
        } else if (prop->type == T_TYPE_SUBSECTION) {
            type = "subsection";
        }

        printf("    %s <%s> (%s)\n", prop->name, type, prop->description?prop->description:"no description");
        if (prop->depends_on) {
            printf("        depending on this: %s.%s\n",
                prop->depends_on->section->name,
                prop->depends_on->name);
        }
        if (prop->defaultvalue) {
            printf("        has default value: %s\n", prop->defaultvalue);
        }
        if (prop->values) {
            printf("        has possible values: %s\n", prop->values);
        }
        if (prop->points_to) {
            printf("        points to this section: %s\n", prop->points_to->name);
        }
        print_property(prop->next);
    }
}

void print_section(struct st_spec_section *sec)
{
    if (sec->type == T_SECTION)
        printf("\n\033[;1mSection: %s\033[0m (%s)\n", sec->name, sec->description);
    else if (sec->type == T_SUBSECTION)
        printf("\n\033[;1mSubsection: %s\033[0m (%s)\n", sec->name, sec->description);
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
    printf("    expr: %s\n", cons->expression);
    print_constraint(cons->next);
}


void bison_parse() {
    st = stack_new(0);
    description = string_new_empty();


    yyparse();

    resolve_symbols();


    //print_section(spec->sections);
    //print_constraint(spec->constraints);
} 

%}
%union {
    char * string;
    int integer;
    double floating;
    struct st_ast * ast;
    enum yytokentype token;
}

/* keywords for sections */
%token T_SECMARKER T_DESCRIPTION T_PROPERTY T_TYPE
%token T_FAILURE T_WARNING T_REJECT T_ASSERT T_DEPENDS_ON T_POINTS_TO T_DEFAULT
%token T_VALUES

/* This are aritmetic tokens */
%token <token> T_ADD T_MULTIPLY T_SUBSTRACT T_NEGATIVE

/* There are the binary tokens */
%token <token> T_OR T_AND T_NOT T_XOR
%token <token> T_LESS T_GREATER T_LESS_EQUAL T_GREATER_EQUAL T_EQUAL T_UNEQUAL

/* types for the properties */
%token <token> T_TYPETOKEN T_TYPE_INTEGER T_TYPE_REAL T_TYPE_BOOLEAN T_TYPE_STRING T_TYPE_SUBSECTION
%token <token> T_SECTION T_SUBSECTION

%token <string> T_HALFNAME T_FULLNAME T_LITERAL

%token <integer> T_INTEGER T_BOOLEAN
%token <floating> T_REAL

%type <ast> expr


//%expect 2

%nonassoc T_NOT
%left T_OR T_AND
%nonassoc T_LESS T_GREATER T_LESS_EQUAL T_GREATER_EQUAL T_EQUAL
%left T_ADD T_SUBSTRACT
%left T_MULTIPLY


%%
spec: /* empty */
    | section spec 
    | failure spec
    | warning spec
    ;

section: sectionheader properties
{
};

sectionheader: newsection description
{
 	activesection->description = strdup(description->content);

    string_free(description);
    description = string_new_empty();
};

warning: warningheader description constraintoptions
{
    activecons->description = strdup(description->content);
    string_free(description);
    description = string_new_empty();
};

failure: failureheader description constraintoptions
{
    activecons->description = strdup(description->content);
    string_free(description);
    description = string_new_empty();
};

failureheader: T_FAILURE
{
    activecons = new_constraint(spec);
    activecons->type = T_FAILURE;
};

warningheader: T_WARNING
{   
    activecons = new_constraint(spec);
    activecons->type = T_WARNING;
};

expr: T_FULLNAME
{
    $$ = new_variable($1);
};
expr: T_INTEGER
{
    $$ = new_constant_integer($1);
};
expr: T_REAL
{
    $$ = new_constant_real($1);
};
expr: T_BOOLEAN
{
    $$ = new_constant_boolean($1);
};
expr: '(' expr ')'
{
    $$ = $2;
};


expr: expr T_ADD expr {$$ = newnode($2, $1, $3);};
expr: expr T_MULTIPLY expr {$$ = newnode($2, $1, $3);};
expr: expr T_SUBSTRACT expr {$$ = newnode($2, $1, $3);};
expr: expr T_AND expr {$$ = newnode($2, $1, $3);};
expr: expr T_XOR expr {$$ = newnode($2, $1, $3);};
expr: expr T_OR expr {$$ = newnode($2, $1, $3);};
expr: expr T_LESS expr {$$ = newnode($2, $1, $3);};
expr: expr T_GREATER expr {$$ = newnode($2, $1, $3);};
expr: expr T_LESS_EQUAL expr {$$ = newnode($2, $1, $3);};
expr: expr T_GREATER_EQUAL expr {$$ = newnode($2, $1, $3);};
expr: expr T_EQUAL expr {$$ = newnode($2, $1, $3);};
expr: expr T_UNEQUAL expr {$$ = newnode($2, $1, $3);};
expr: T_NOT expr {$$ = newnode($1, $2, NULL);};
expr: T_SUBSTRACT expr {$$ = newnode(T_NEGATIVE, $2, NULL);};

properties
    : /* empty */
    | properties property 
    ;

property: beginproperty options 
{
};

beginproperty: T_PROPERTY T_HALFNAME
{
    activeproperty = new_property(activesection);
    activeproperty->name = $2;
};

constraintoptions
    : /* empty */
    | constraintoptions constraintoption
    ;

constraintoption: T_DEPENDS_ON T_FULLNAME
{
    queue_symbol($2, &activecons->depends_on, T_PROPERTY);
};
constraintoption: T_REJECT T_LITERAL expr
{
    activecons->ast = $3;
    activecons->expression = $2;
    activecons->flavour = T_REJECT;
};
constraintoption: T_ASSERT T_LITERAL expr
{
    activecons->ast = $3;
    activecons->expression = $2;
    activecons->flavour = T_ASSERT;
};

options
    : 
    | option options
    ;

option: T_TYPE T_TYPETOKEN
{
    activeproperty->type = $2;
};
option: T_DEPENDS_ON T_FULLNAME
{
    queue_symbol($2, &activeproperty->depends_on, T_PROPERTY);
};
option: T_POINTS_TO T_HALFNAME
{
    queue_symbol($2, &activeproperty->points_to, T_SECTION);
};
option: description
{
    activeproperty->description = strdup(description->content);
    string_free(description);
    description = string_new_empty();
};
option: T_DEFAULT T_LITERAL
{
    activeproperty->defaultvalue = $2;
};
option: T_VALUES T_LITERAL
{
    activeproperty->values = $2;
};

newsection: T_SECTION T_HALFNAME
{
    activesection = new_section(spec);
    activesection->name = $2;
    activesection->type = $1;
};

description
    : descriptionline
    | description descriptionline
    ;

descriptionline: T_DESCRIPTION T_LITERAL 
{
    string_cat_array(description, $2);
    free($2);
};
