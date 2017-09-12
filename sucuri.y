%require "3.0"

%language "c++"
%skeleton "lalr1.cc"

%token NEWLINE INDENT DEDENT
%token NOT POW MUL DIV PLUS MINUS LT LE GT GE EQ NE AND OR XOR
%token AS CATCH CLASS ELSE EXPORT FOR FROM IF IMPORT IN LET RETURN THROW TRY WHILE
%token IDENTIFIER ELLIPSIS

%token COMMA ","
%token END 0
%token LPAREN "("
%token RPAREN ")"
%token LBRACK "["
%token RBRACK "]"

%defines "parser.hxx"
%output "parser.cxx"
%verbose

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.error verbose
%define parse.trace

%token <long double> FLOAT
%token <long long> INTEGER
%token <std::string> STRING

%locations
%param {parser::semantic_type& yylval} {parser::location_type& yylloc}

%code {
yy::parser::symbol_type yylex(yy::parser::semantic_type& yylval, yy::parser::location_type& yylloc);
}

%start code

%%

code
    : program NEWLINE
    | imports program NEWLINE;

atom
    : IDENTIFIER
    | INTEGER
    | FLOAT
    | STRING
    | LPAREN expr RPAREN
    | LBRACK list_expr RBRACK

/* expressions */
atom_expr
    : atom
    | atom trailer;

trailer
    : LPAREN RPAREN
    | LPAREN arglist RPAREN
    | LBRACK expr RBRACK
    | '.' IDENTIFIER;

list_expr
    : atom
    | list_expr COMMA atom;

exponential_expr
    : atom_expr
    | exponential_expr POW atom_expr;

unary_expr
    : NOT unary_expr
    | MINUS unary_expr
    | exponential_expr;

multiplicative_expr
    : unary_expr
    | multiplicative_expr MUL unary_expr
    | multiplicative_expr DIV unary_expr;

additive_expr
    : multiplicative_expr
    | additive_expr PLUS multiplicative_expr
    | additive_expr MINUS multiplicative_expr;

relational_expr
    : additive_expr
    | relational_expr LT additive_expr
    | relational_expr LE additive_expr
    | relational_expr GT additive_expr
    | relational_expr GE additive_expr;

equality_expr
    : relational_expr
    | equality_expr EQ relational_expr
    | equality_expr NE relational_expr;

logical_expr
    : equality_expr
    | logical_expr AND equality_expr
    | logical_expr OR equality_expr
    | logical_expr XOR equality_expr;

expr
    : logical_expr;

exprlist
    : expr
    | exprlist COMMA expr;

/* module system */
imports
    : import_stmt NEWLINE
    | imports import_stmt NEWLINE;

import_stmt
    : IMPORT dotted_as_names
    | IMPORT LPAREN dotted_as_names RPAREN
    | FROM dotted_name IMPORT import_as_names
    | FROM dotted_name IMPORT LPAREN import_as_names RPAREN;

/* import a.b.c */
dotted_as_names
    : dotted_as_name
    | dotted_as_names COMMA dotted_as_name;

dotted_as_name
    : dotted_name
    | dotted_name AS IDENTIFIER;

/* from a.b import c */
import_as_names
    : import_as_name
    | import_as_names COMMA import_as_name;

import_as_name
    : IDENTIFIER
    | IDENTIFIER AS IDENTIFIER;

dotted_name
    : IDENTIFIER
    | dotted_name '.' IDENTIFIER;

program
    : stmt NEWLINE
    | definition NEWLINE
    | stmt NEWLINE program
    | definition NEWLINE program;

definition
    : function_definition
    | class_definition
    | EXPORT function_definition
    | EXPORT class_definition;


function_definition
    : LET IDENTIFIER LPAREN RPAREN scope
    | LET IDENTIFIER LPAREN function_params_list RPAREN scope;

function_params_list
    : identifier_list
    | identifier_list COMMA variadic_param
    | variadic_param;

identifier_list
    : IDENTIFIER
    | IDENTIFIER EQ atom
    | identifier_list COMMA IDENTIFIER
    | identifier_list COMMA IDENTIFIER EQ atom;

variadic_param
    : ELLIPSIS IDENTIFIER;

scope
    : NEWLINE INDENT inner_scope DEDENT;

inner_scope
    : stmt NEWLINE
    | inner_scope stmt NEWLINE;


class_definition
    : CLASS IDENTIFIER class_scope;

class_scope
    : NEWLINE INDENT inner_class_scope DEDENT;

inner_class_scope
    : function_definition
    | inner_class_scope function_definition;

stmt
    : assignment_expr
    | function_call
    | compound_stmt
    | THROW expr
    | RETURN exprlist;

assignment_expr
    : LET IDENTIFIER EQ atom_expr
    | IDENTIFIER EQ atom_expr
    | IDENTIFIER LBRACK atom RBRACK EQ atom_expr;

function_call
    : IDENTIFIER LPAREN RPAREN
    | IDENTIFIER LPAREN arglist RPAREN;

arglist
    : atom
    | arglist COMMA atom;

/* flow control */
compound_stmt
    : if_stmt
    | while_stmt
    | for_stmt
    | try_stmt;

if_stmt
    : IF expr scope
    | IF expr scope ELSE scope;

while_stmt
    : WHILE expr scope;

for_stmt
    : FOR exprlist IN expr scope;

try_stmt
    : TRY scope
    | TRY scope CATCH expr
    | TRY scope CATCH expr AS IDENTIFIER;

%%

void yy::parser::error(const yy::location& loc, const std::string& message)
{
    std::cout << "Error: " << message << "\nLocation: " << loc << '\n';
}

/* int yylex(void) */
/* { */
/*     std::cout << "Enfia essa função no cu" << std::endl; */
/*     return 0; */
/* } */

/* vim: set ft=yacc: */
