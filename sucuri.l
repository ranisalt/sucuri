%{

#include "parser.hxx"
#include "scanner.h"
#include <stack>

unsigned nesting = 0;
std::stack<int> indent_stack{{0}};

void count(const char*);
#define yyterminate() return yy::parser::make_END(loc)

static yy::location loc;

%}

%option c++

%option header-file="scanner.hxx"
%option outfile="scanner.cxx"
%option yyclass="yy::scanner"

%option yylineno
%option noinput nounput noyywrap

%x indent
%s normal

D [0-9]
L [a-zA-Z_!@$?]
NO_SQUOTE_STRING_LITERAL [^']*
NO_DQUOTE_STRING_LITERAL [^"]*

%%

%{
BEGIN indent;
%}

<indent>[ \t]*/[^ \n\r\t] {
    BEGIN normal;
    if (yyleng > indent_stack.top()) {
        indent_stack.push(yyleng);
        std::cout << "INDENT\n";
        return yy::parser::make_INDENT(loc);
    } else if (yyleng < indent_stack.top()) {
        while (yyleng < indent_stack.top()) {
            indent_stack.pop();
            std::cout << "DEDENT\n";
            return yy::parser::make_DEDENT(loc);
        }
    }
}

[\n\r] {
    if (nesting == 0) {
        BEGIN indent;
        std::cout << "Back to indent.\n";
        return yy::parser::make_NEWLINE(loc);
    }
}

"#"[^\n]*\n { count("COMMENT"); /* ignore comments */ }

    /* literals */
[+-]?{D}+   {
    /* yylval->build(atoll(yytext)); */
    count("INTEGER_LITERAL");
    return yy::parser::make_INTEGER_LITERAL(loc);
}

[+-]"."{D}+     {
    /* yylval->build(atof(yytext)); */
    count("FLOAT_LITERAL");
    return yy::parser::make_FLOAT_LITERAL(loc);
}

[+-]{D}+"."{D}* {
    /* yylval->build(atof(yytext)); */
    count("FLOAT_LITERAL");
    return yy::parser::make_FLOAT_LITERAL(loc);
}

"'"{NO_SQUOTE_STRING_LITERAL}"'" {
    count("STRING_LITERAL");
    return yy::parser::make_STRING_LITERAL(loc);
}

"\""{NO_DQUOTE_STRING_LITERAL}"\"" {
    count("STRING_LITERAL");
    return yy::parser::make_STRING_LITERAL(loc);
}

    /* operators */
"!="        { count("NE"); return yy::parser::make_NE(loc); }
"("         { count("LPAREN"); ++nesting; return yy::parser::make_LPAREN(loc); }
")"         { count("RPAREN"); --nesting; return yy::parser::make_RPAREN(loc); }
"*"         { count("MUL"); return yy::parser::make_MUL(loc); }
"**"        { count("POW"); return yy::parser::make_POW(loc); }
"+"         { count("PLUS"); return yy::parser::make_PLUS(loc); }
"-"         { count("MINUS"); return yy::parser::make_MINUS(loc); }
"/"         { count("DIV"); return yy::parser::make_DIV(loc); }
"<"         { count("LT"); return yy::parser::make_LT(loc); }
"<="        { count("LE"); return yy::parser::make_LE(loc); }
"="         { count("EQ"); return yy::parser::make_EQ(loc); }
">"         { count("GT"); return yy::parser::make_GT(loc); }
">="        { count("GE"); return yy::parser::make_GE(loc); }
"and"       { count("AND"); return yy::parser::make_AND(loc); }
"not"       { count("NOT"); return yy::parser::make_NOT(loc); }
"or"        { count("OR"); return yy::parser::make_OR(loc); }
"xor"       { count("XOR"); return yy::parser::make_XOR(loc); }

    /* keywords */
"as"        { count("AS"); return yy::parser::make_AS(loc); }
"catch"     { count("CATCH"); return yy::parser::make_CATCH(loc); }
"class"     { count("CLASS"); return yy::parser::make_CLASS(loc); }
"else"      { count("ELSE"); return yy::parser::make_ELSE(loc); }
"export"    { count("EXPORT"); return yy::parser::make_EXPORT(loc); }
"for"       { count("FOR"); return yy::parser::make_FOR(loc); }
"from"      { count("FROM"); return yy::parser::make_FROM(loc); }
"if"        { count("IF"); return yy::parser::make_IF(loc); }
"import"    { count("IMPORT"); return yy::parser::make_IMPORT(loc); }
"in"        { count("IN"); return yy::parser::make_IN(loc); }
"let"       { count("LET"); return yy::parser::make_LET(loc); }
"return"    { count("RETURN"); return yy::parser::make_RETURN(loc); }
"throw"     { count("THROW"); return yy::parser::make_THROW(loc); }
"try"       { count("TRY"); return yy::parser::make_TRY(loc); }
"while"     { count("WHILE"); return yy::parser::make_WHILE(loc); }

    /* extra */
"..."                 { count("ELLIPSIS"); return yy::parser::make_ELLIPSIS(loc); }
{L}((\.{L})|{L}|{D})* { count("IDENTIFIER"); return yy::parser::make_IDENTIFIER(loc); }

.           { /* ignore extraneous characters */ }

%%

int column = 0;

void count(const char* token)
{
    /* std::cout << token << ": " << yytext << "\n"; */
}

/* vim: set ft=lex: */
