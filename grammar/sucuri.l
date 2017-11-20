%{

#include "parser.hxx"
#include <stack>

unsigned nesting = 0;
std::stack<int> indent_stack{{0}};

void count(const char*);
static yy::location yylloc;
#define yyterminate() return yy::parser::make_END(yylloc)
#define YY_DECL yy::parser::symbol_type yylex(yy::parser::semantic_type& yylval, yy::parser::location_type& yylloc)
#define YY_USER_ACTION yylloc.columns(yyleng);

long double text_to_float(const char* s) { return std::strtold(s, nullptr); }
long long text_to_integer(const char* s) { return std::strtoll(s, nullptr, 10); }

%}

%option verbose

%option yylineno

%option nodefault noinput nounput noyywrap

%x INDENT
%x COMMENT

D [0-9]
L [a-zA-Z_!@$?]
NO_SQUOTE_STRING [^']*
NO_DQUOTE_STRING [^"]*

%%

%{
static bool called = false;
if (!called) {
    called = true;
    BEGIN INDENT;
}
%}

<INDENT>[ \t]*/[^ \t\n\r#] {
    if (yyleng > indent_stack.top()) {
        indent_stack.push(yyleng);
        BEGIN INITIAL;
        return yy::parser::make_INDENT(yylloc);
    } else if (yyleng < indent_stack.top()) {
        indent_stack.pop();
        if (yyleng == indent_stack.top()) {
            BEGIN INITIAL;
            return yy::parser::make_DEDENT(yylloc);
        } else {
            return yy::parser::make_DEDENT(yylloc);
        }
    } else {
        BEGIN INITIAL;
    }
}

<INDENT>#[^\n\r]*
<INDENT>[\n\r] { yylloc.lines(1); }

<INITIAL>#[^\n\r]*
<INITIAL>[\n\r] {
    /* */
    yylloc.lines(1);
    if (nesting == 0) {
        BEGIN INDENT;
        /* return yy::parser::make_NEWLINE(yylloc); */
    }
}

    /* literals */
[+-]?"."{D}+ {
    auto val = text_to_float(yytext);
    return yy::parser::make_FLOAT(val, yylloc);
}

[+-]?{D}+"."{D}* {
    auto val = text_to_float(yytext);
    return yy::parser::make_FLOAT(val, yylloc);
}

[+-]?{D}+ {
    auto val = text_to_integer(yytext);
    return yy::parser::make_INTEGER(val, yylloc);
}

true {
    auto val = true;
    return yy::parser::make_BOOL(val, yylloc);
}

false {
    auto val = false;
    return yy::parser::make_BOOL(val, yylloc);
}

"'"{NO_SQUOTE_STRING}"'" {
    auto val = std::string{yytext+1u, yyleng-2u};
    return yy::parser::make_STRING(val, yylloc);
}

"\""{NO_DQUOTE_STRING}"\"" {
    auto val = std::string{yytext+1u, yyleng-2u};
    return yy::parser::make_STRING(val, yylloc);
}

    /* operators */
"!="  { return yy::parser::make_NE(yylloc); }
"("   { ++nesting; return yy::parser::make_LPAREN(yylloc); }
")"   { --nesting; return yy::parser::make_RPAREN(yylloc); }
"["   { ++nesting; return yy::parser::make_LBRACK(yylloc); }
"]"   { --nesting; return yy::parser::make_RBRACK(yylloc); }
"*"   { return yy::parser::make_MUL(yylloc); }
"**"  { return yy::parser::make_POW(yylloc); }
"+"   { return yy::parser::make_PLUS(yylloc); }
"-"   { return yy::parser::make_MINUS(yylloc); }
"/"   { return yy::parser::make_DIV(yylloc); }
"<"   { return yy::parser::make_LT(yylloc); }
"<="  { return yy::parser::make_LE(yylloc); }
"="   { return yy::parser::make_EQ(yylloc); }
">"   { return yy::parser::make_GT(yylloc); }
">="  { return yy::parser::make_GE(yylloc); }
"and" { return yy::parser::make_AND(yylloc); }
"not" { return yy::parser::make_NOT(yylloc); }
"or"  { return yy::parser::make_OR(yylloc); }
"xor" { return yy::parser::make_XOR(yylloc); }

    /* keywords */
"as"     { return yy::parser::make_AS(yylloc); }
"catch"  { return yy::parser::make_CATCH(yylloc); }
"class"  { return yy::parser::make_CLASS(yylloc); }
"else"   { return yy::parser::make_ELSE(yylloc); }
"export" { return yy::parser::make_EXPORT(yylloc); }
"for"    { return yy::parser::make_FOR(yylloc); }
"from"   { return yy::parser::make_FROM(yylloc); }
"if"     { return yy::parser::make_IF(yylloc); }
"import" { return yy::parser::make_IMPORT(yylloc); }
"in"     { return yy::parser::make_IN(yylloc); }
"let"    { return yy::parser::make_LET(yylloc); }
"return" { return yy::parser::make_RETURN(yylloc); }
"throw"  { return yy::parser::make_THROW(yylloc); }
"try"    { return yy::parser::make_TRY(yylloc); }
"while"  { return yy::parser::make_WHILE(yylloc); }

    /* extra */
"..."                 { return yy::parser::make_ELLIPSIS(yylloc); }
{L}((\.{L})|{L}|{D})* { return yy::parser::make_IDENTIFIER(yytext, yylloc); }

","   { return yy::parser::make_COMMA(yylloc); }
" "+
<INDENT><<EOF>> {
    // pop remaining indent levels
    if (indent_stack.top() != 0) {
        indent_stack.pop();
        return yy::parser::make_DEDENT(yylloc);
    } else {
        BEGIN INITIAL;
    }
}

<<EOF>> {
    if (indent_stack.top() != 0) {
        BEGIN INDENT;
    } else {
        yyterminate();
    }
}

. { }

%%

/* vim: set ft=lex: */
