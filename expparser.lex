%option noyywrap
%{
#include <stdio.h>
#include "expparser.tab.h"
%}
%x literal type key halfname expresion


N [0-9]
INT {N}+
FLOAT {INT}\.{INT}?
OPERATION (OR|AND|<|>|[+]|[*]|-)

HALFNAME [a-zA-Z0-9_]+
FULLNAME {HALFNAME}\.{HALFNAME}
TYPE (int|double|bool|string|subsection)
%%

[ \t\n]+ ;
^#.* ;
TYPE {
    BEGIN(type);
    return TYPE;
}
DEPENDS_ON {printf("(%s)", yytext); BEGIN(key);}

SUBSECTION|SECTION {
    BEGIN(halfname);
    return SECTION;
}

FAILURE {
    return FAILURE;
}
WARNING {
    return WARNING;
}
CONDITION {
    BEGIN(expresion);
    return CONDITION;
}
PROPERTY {
    BEGIN(halfname);
    return PROPERTY;
}

DESCRIPTION {
    BEGIN(literal);
    return DESCRIPTION;
}

<literal>[ \t]+ ;
<literal>[^ \t\n]+.* {BEGIN(INITIAL);yylval.string=strdup(yytext);return LITERAL;}

<type>{
    [ \t]+ ;
{TYPE} {
    yylval.string=strdup(yytext);
    return TYPETOKEN;
}
\n {
    BEGIN(INITIAL);
}
}

<halfname>{
[ \t]+ ;
{HALFNAME} {
    BEGIN(INITIAL);
    yylval.string=strdup(yytext);
    return HALFNAME;
}
}

<expresion>{
[ \t]+ ;

{INT} {
    yylval.integer = atoi(yytext);
    return INTEGER;
}

{FLOAT} {
    yylval.floating = strtod(yytext, NULL);
    return FLOAT;
}

{OPERATION} {
    yylval.string = strdup(yytext);

    return OP;
}

{FULLNAME} {
    yylval.string = strdup(yytext);

    return KEY;
}

[(] { return '('; }
[)] { return ')'; }
\n {
    BEGIN(INITIAL);
}
}

%%
