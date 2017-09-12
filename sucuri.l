%{

#include "parser.hxx"
/* #include "scanner.h" */
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

/* %option c++ */
%option verbose

%option header-file="scanner.hxx"
%option outfile="scanner.cxx"
/* %option yyclass="yy::scanner" */

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
    std::cout << "Sua mãe\n";
}
%}

<INDENT>[ \t]*/[^ \t\n\r#] {
    BEGIN INITIAL;
    if (yyleng > indent_stack.top()) {
        count("INDENT");
        indent_stack.push(yyleng);
        return yy::parser::make_INDENT(yylloc);
    } else if (yyleng < indent_stack.top()) {
        while (yyleng < indent_stack.top()) {
            count("DEDENT");
            indent_stack.pop();
            return yy::parser::make_DEDENT(yylloc);
        }
    }
}

<INDENT>[\n\r]

[\n\r] {
    if (nesting == 0) {
        count("NEWLINE");
        BEGIN INDENT;
        return yy::parser::make_NEWLINE(yylloc);
    }
}

"#"[^\n\r]*

    /* literals */
[+-]?"."{D}+ {
    auto val = text_to_float(yytext);
    /* yylval.build<long double>(val); */
    count("FLOAT");
    return yy::parser::make_FLOAT(val, yylloc);
}

[+-]?{D}+"."{D}* {
    auto val = text_to_float(yytext);
    /* yylval.build<long double>(text_to_float(yytext)); */
    count("FLOAT");
    return yy::parser::make_FLOAT(val, yylloc);
}

[+-]?{D}+ {
    auto val = text_to_integer(yytext);
    /* yylval.build<long long>(text_to_integer(yytext)); */
    count("INTEGER");
    return yy::parser::make_INTEGER(val, yylloc);
}

"'"{NO_SQUOTE_STRING}"'" {
    auto val = std::string{yytext+1u, yyleng-2u};
    /* yylval.build<std::string>({yytext+1u, yyleng-2u}); */
    count("STRING");
    return yy::parser::make_STRING(val, yylloc);
}

"\""{NO_DQUOTE_STRING}"\"" {
    auto val = std::string{yytext+1u, yyleng-2u};
    /* yylval.build<std::string>({yytext+1u, yyleng-2u}); */
    count("STRING");
    return yy::parser::make_STRING(val, yylloc);
}

    /* operators */
"!="  { count("NE"); return yy::parser::make_NE(yylloc); }
"("   { count("LPAREN"); ++nesting; return yy::parser::make_LPAREN(yylloc); }
")"   { count("RPAREN"); --nesting; return yy::parser::make_RPAREN(yylloc); }
"["   { count("LBRACK"); ++nesting; return yy::parser::make_LBRACK(yylloc); }
"]"   { count("RBRACK"); --nesting; return yy::parser::make_RBRACK(yylloc); }
"*"   { count("MUL"); return yy::parser::make_MUL(yylloc); }
"**"  { count("POW"); return yy::parser::make_POW(yylloc); }
"+"   { count("PLUS"); return yy::parser::make_PLUS(yylloc); }
"-"   { count("MINUS"); return yy::parser::make_MINUS(yylloc); }
"/"   { count("DIV"); return yy::parser::make_DIV(yylloc); }
"<"   { count("LT"); return yy::parser::make_LT(yylloc); }
"<="  { count("LE"); return yy::parser::make_LE(yylloc); }
"="   { count("EQ"); return yy::parser::make_EQ(yylloc); }
">"   { count("GT"); return yy::parser::make_GT(yylloc); }
">="  { count("GE"); return yy::parser::make_GE(yylloc); }
"and" { count("AND"); return yy::parser::make_AND(yylloc); }
"not" { count("NOT"); return yy::parser::make_NOT(yylloc); }
"or"  { count("OR"); return yy::parser::make_OR(yylloc); }
"xor" { count("XOR"); return yy::parser::make_XOR(yylloc); }

    /* keywords */
"as"     { count("AS"); return yy::parser::make_AS(yylloc); }
"catch"  { count("CATCH"); return yy::parser::make_CATCH(yylloc); }
"class"  { count("CLASS"); return yy::parser::make_CLASS(yylloc); }
"else"   { count("ELSE"); return yy::parser::make_ELSE(yylloc); }
"export" { count("EXPORT"); return yy::parser::make_EXPORT(yylloc); }
"for"    { count("FOR"); return yy::parser::make_FOR(yylloc); }
"from"   { count("FROM"); return yy::parser::make_FROM(yylloc); }
"if"     { count("IF"); return yy::parser::make_IF(yylloc); }
"import" { count("IMPORT"); return yy::parser::make_IMPORT(yylloc); }
"in"     { count("IN"); return yy::parser::make_IN(yylloc); }
"let"    { count("LET"); return yy::parser::make_LET(yylloc); }
"return" { count("RETURN"); return yy::parser::make_RETURN(yylloc); }
"throw"  { count("THROW"); return yy::parser::make_THROW(yylloc); }
"try"    { count("TRY"); return yy::parser::make_TRY(yylloc); }
"while"  { count("WHILE"); return yy::parser::make_WHILE(yylloc); }

    /* extra */
"..."                 { count("ELLIPSIS"); return yy::parser::make_ELLIPSIS(yylloc); }
{L}((\.{L})|{L}|{D})* { count("IDENTIFIER"); return yy::parser::make_IDENTIFIER(yylloc); }

","   { return yy::parser::make_COMMA(yylloc); }
" "+
.     { count("UNMATCHED"); }

%%

void count(const char* token)
{
    std::cout << std::string(">>", 2*indent_stack.size()) << ' ' << token << ": '" << yytext << "'\n";
}

/* vim: set ft=lex: */
