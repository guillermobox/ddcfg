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
DEPENDS_ON {
    BEGIN(fullname);
    return T_DEPENDS_ON;
}
POINTS_TO {
    BEGIN(fullname);
    return T_POINTS_TO;
}
SUBSECTION|SECTION {
    BEGIN(halfname);
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
        return T_FLOAT;
    }

    {OPERATION} {
        yylval.string = strdup(yytext);

        return T_OP;
    }

    {FULLNAME} {
        yylval.string = strdup(yytext);

        return T_KEY;
    }

    [(] { return '('; }
    [)] { return ')'; }
    \n {
        BEGIN(INITIAL);
    }
}

%%
