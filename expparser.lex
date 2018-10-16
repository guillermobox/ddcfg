%option noyywrap
%{
#include <stdio.h>
#include "y.tab.h"
extern char * yylval;
%}
%x literal type key halfname

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

PROPERTY {
    BEGIN(halfname);
    return PROPERTY;
}

DESCRIPTION {
    BEGIN(literal);
    return DESCRIPTION;
}

<literal>[ \t]+ ;
<literal>[^ \t\n]+.* {BEGIN(INITIAL);yylval=strdup(yytext);return LITERAL;}

<type>{
    [ \t]+ ;
{TYPE} {
    yylval=strdup(yytext);
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
    yylval=strdup(yytext);
    return HALFNAME;
}
}

%%
