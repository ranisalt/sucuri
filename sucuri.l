D [0-9]
L [a-zA-Z_!@$?]
NO_SQUOTE_STRING_LITERAL [^']*
NO_DQUOTE_STRING_LITERAL [^"]*

%{

#include <assert.h>
#include <stdio.h>
#include "sucuri.tab.h"

void count();

#define MAX_DEPTH 72
unsigned nesting = 0;
unsigned indent_stack[MAX_DEPTH] = {0};
unsigned level = 0;
void process_indent(unsigned leng);
void process_newline(void);

%}

%option noyywrap

%%

^[ ]*\n     { /* ignore empty lines */ }
^[ ]*       { process_indent(yyleng); }

"#"[^\n]*\n { /* ignore comments */ }

    /* literals */
{D}+        { count(); return INTEGER_LITERAL; }
"."{D}+     { count(); return FLOAT_LITERAL; }
{D}+"."{D}* { count(); return FLOAT_LITERAL; }
"'"{NO_SQUOTE_STRING_LITERAL}"'" { count(); return STRING_LITERAL; }
"\""{NO_DQUOTE_STRING_LITERAL}"\"" { count(); return STRING_LITERAL; }

    /* operators */
"!="        { count(); return NE; }
"("         { count(); ++nesting; return '('; }
")"         { count(); --nesting; return ')'; }
"*"         { count(); return MUL; }
"**"        { count(); return POW; }
"+"         { count(); return ADD; }
"-"         { count(); return SUB; }
"/"         { count(); return DIV; }
"<"         { count(); return LT; }
"<="        { count(); return LE; }
"="         { count(); return EQ; }
">"         { count(); return GT; }
">="        { count(); return GE; }
"and"       { count(); return AND; }
"not"       { count(); return NOT; }
"or"        { count(); return OR; }
"xor"       { count(); return XOR; }

    /* keywords */
"as"        { count(); return AS; }
"catch"     { count(); return CATCH; }
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
"try"       { count(); return TRY; }
"while"     { count(); return WHILE; }

    /* extra */
"..."                 { count(); return ELLIPSIS; }
{L}((\.{L})|{L}|{D})* { count(); return IDENTIFIER; }

.           { /* ignore extraneous characters */ }

%%

void process_indent(unsigned leng)
{
    if (nesting == 0) {
        if (leng > indent_stack[level]) {
            assert(indent_stack[level] + 1 < MAX_DEPTH);
            indent_stack[++level] = leng;
            unput(INDENT);
        } else if (leng < indent_stack[level]) {
            while (leng < indent_stack[level]) {
                --level;
                unput(DEDENT);
            }
        }
    }
}

void process_newline(void)
{
    if (nesting == 0) {
        unput(NEWLINE);
        printf("\n");
    }
}

int column = 0;

void count(void) {
    for (int i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            process_newline();
            column = 0;
        } else if (yytext[i] == '\t') {
            column += 8 - (column % 8);
        } else {
            column++;
        }
    }

    ECHO;
}
