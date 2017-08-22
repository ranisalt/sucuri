D [0-9]
L [a-zA-Z_!@$?]
NO_SQUOTE_STRING_LITERAL [^']*
NO_DQUOTE_STRING_LITERAL [^"]*

%{

#include <stdio.h>
#include "sucuri.tab.h"

void count();
void indent_level();

%}

%%

"#"[^\n\r]*[\n\r] { printf("comment :3\n"); }

    /* literals */
{D}+        { count(); return INTEGER_LITERAL; }
{D}*"."{D}+ { count(); return FLOAT_LITERAL; }
{D}+"."{D}* { count(); return FLOAT_LITERAL; }
"'"{NO_SQUOTE_STRING_LITERAL}"'" { count(); return STRING_LITERAL; }
"\""{NO_DQUOTE_STRING_LITERAL}"\"" { count(); return STRING_LITERAL; }

    /* operators */
"!="        { count(); return NE; }
"("         { count(); return '('; }
")"         { count(); return ')'; }
"*"         { count(); return '*'; }
"**"        { count(); return POW; }
"+"         { count(); return '+'; }
"-"         { count(); return '-'; }
"/"         { count(); return '/'; }
"<"         { count(); return '<'; }
"<="        { count(); return LE; }
"="         { count(); return '='; }
">"         { count(); return '>'; }
">="        { count(); return GE; }
"and"       { count(); return AND; }
"not"       { count(); return NOT; }
"or"        { count(); return OR; }
"xor"       { count(); return XOR; }

    /* keywords */
"as"        { count(); return AS; }
"class"     { count(); return CLASS; }
"else"      { count(); return ELSE; }
"export"    { count(); return EXPORT; }
"for"       { count(); return FOR; }
"from"      { count(); return FROM; }
"if"        { count(); return IF; }
"import"    { count(); return IMPORT; }
"in"        { count(); return IN; }
"let"       { count(); return LET; }
"return"    { count(); return RETURN; }
"throw"     { count(); return THROW; }
"while"     { count(); return WHILE; }

    /* extra */
"..."                 { count(); return ELLIPSIS; }
{L}((\.{L})|{L}|{D})* { count(); return IDENTIFIER; }

%%

int yywrap(void) {
    printf("~~~ end of parse ~~~\n");
    return 1;
}

void indent_level() {
}

int column = 0;

void count() {
    for (int i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            column = 0;
        } else if (yytext[i] == '\t') {
            column += 8 - (column % 8);
        } else {
            column++;
        }
    }

    printf("found ");
    ECHO;
    printf("\n");
}
