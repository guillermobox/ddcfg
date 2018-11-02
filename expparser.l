%option noyywrap
%{
#include <stdio.h>
#include "expparser.tab.h"
%}
%option yylineno

%x literal type key halfname expresion literalexpresion fullname

N [0-9]
INT {N}+
FLOAT {INT}\.{INT}?
OPERATION (OR|AND|<|>|[+]|[*]|-)
MONOOPERATION (NOT)

HALFNAME [a-zA-Z0-9_]+
FULLNAME {HALFNAME}\.{HALFNAME}
TYPE (integer|real|boolean|string|subsection)
%%

[ \t\n]+ ; /* skip empty lines and also space at the begining of line */
^#.* ; /* skip comments */

    /* possible keywords to have at the begining of each line */
TYPE {
    BEGIN(type);
    return T_TYPE;
}
DEFAULT {
    BEGIN(literal);
    return T_DEFAULT;
}
VALUES {
    BEGIN(literal);
    return T_VALUES;
}
DEPENDS_ON {
    BEGIN(fullname);
    return T_DEPENDS_ON;
}
POINTS_TO {
    BEGIN(halfname);
    return T_POINTS_TO;
}
SECTION {
    BEGIN(halfname);
    yylval.token = T_SECTION;
    return T_SECTION;
}
SUBSECTION {
    BEGIN(halfname);
    yylval.token = T_SUBSECTION;
    return T_SECTION;
}
FAILURE {
    return T_FAILURE;
}
WARNING {
    return T_WARNING;
}
CONDITION {
    BEGIN(literalexpresion);
    return T_CONDITION;
}
PROPERTY {
    BEGIN(halfname);
    return T_PROPERTY;
}
DESCRIPTION {
    BEGIN(literal);
    return T_DESCRIPTION;
}
<literal>[ \t]+ ;
<literal>[^ \t\n]+.* {BEGIN(INITIAL);yylval.string=strdup(yytext);return T_LITERAL;}

<type>{
    [ \t]+ ;
    "integer"    {yylval.token = T_TYPE_INTEGER; return T_TYPETOKEN;}
    "real"       {yylval.token = T_TYPE_REAL; return T_TYPETOKEN;}
    "boolean"    {yylval.token = T_TYPE_BOOLEAN; return T_TYPETOKEN;}
    "string"     {yylval.token = T_TYPE_STRING; return T_TYPETOKEN;}
    "subsection" {yylval.token = T_TYPE_SUBSECTION; return T_TYPETOKEN;}
    \n {
        BEGIN(INITIAL);
    }
}

<fullname>{
    [ \t]+ ;
    {FULLNAME} {
        BEGIN(INITIAL);
        yylval.string = strdup(yytext);
        return T_FULLNAME;
    }
}

<halfname>{
    [ \t]+ ;
    {HALFNAME} {
        BEGIN(INITIAL);
        yylval.string=strdup(yytext);
        return T_HALFNAME;
    }
}

<literalexpresion>{ 
    [ \t]+ {;}
    [^ \t].+ {
        yylval.string = strdup(yytext);
        yyless(0);
        BEGIN(expresion);
        return T_LITERAL;
    }
}

<expresion>{
    [ \t]+ {;}
    {INT} {
        yylval.integer = atoi(yytext);
        return T_INTEGER;
    }

    {FLOAT} {
        yylval.floating = strtod(yytext, NULL);
        return T_REAL;
    }

    {FULLNAME} {
        yylval.string = strdup(yytext);
        return T_FULLNAME;
    }

    true|TRUE|yes|YES {
        yylval.integer = 1;
        return T_BOOLEAN;
    }

    false|FALSE|no|NO {
        yylval.integer = 0;
        return T_BOOLEAN;
    }

    "OR"    yylval.token = T_OR; return T_OR;
    "AND"   yylval.token = T_AND; return T_AND;
    "NOT"   yylval.token = T_NOT; return T_NOT;
    "XOR"   yylval.token = T_XOR; return T_XOR;

    "+"     yylval.token = T_ADD; return T_ADD;
    "*"     yylval.token = T_MULTIPLY; return T_MULTIPLY;
    "-"     yylval.token = T_SUBSTRACT; return T_SUBSTRACT;

    "<"     yylval.token = T_LESS; return T_LESS;
    ">"     yylval.token = T_GREATER; return T_GREATER;
    "<="    yylval.token = T_LESS_EQUAL; return T_LESS_EQUAL;
    ">="    yylval.token = T_GREATER_EQUAL; return T_GREATER_EQUAL;
    "=="    yylval.token = T_EQUAL; return T_EQUAL;
    "!="    yylval.token = T_UNEQUAL; return T_UNEQUAL;

    [(] { return '('; }
    [)] { return ')'; }
    \n {
        BEGIN(INITIAL);
    }
}

. {
    printf("at line %d, Unrecognized character!\n", yylineno);
    exit(1);
}
%%
