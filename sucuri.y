%require "3.0"

%language "c++"
%skeleton "lalr1.cc"

%token INDENT DEDENT
%token NOT POW MUL DIV PLUS MINUS LT LE GT GE EQ NE AND OR XOR
%token AS CATCH CLASS ELSE EXPORT FOR FROM IF IMPORT IN LET RETURN THROW TRY WHILE
%token ELLIPSIS

%token END 0
%token COMMA ","
%token LPAREN "("
%token RPAREN ")"
%token LBRACK "["
%token RBRACK "]"

%left MUL
%left DIV
%left PLUS
%left MINUS
%left LT
%left LE
%left GT
%left GE
%left EQ
%left NE
%left AND
%left OR
%left XOR

%defines "parser.hxx"
%output "parser.cxx"
%verbose

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.error verbose
%define parse.trace

/* %union { */
/*   Node* node; */
/*   Literal* literal; */
/* } */

%token <long double> FLOAT
%token <long long> INTEGER
%token <std::string> STRING
%token <std::string> IDENTIFIER

%locations
%param {parser::semantic_type& yylval} {parser::location_type& yylloc}

%code requires {
#include "ast.h"

using namespace AST;
}

%code {
yy::parser::symbol_type yylex(
  yy::parser::semantic_type& yylval,
  yy::parser::location_type& yylloc
);
}

/* %type <std::shared_ptr<Program>> program */

/* %type <ImportList> import_list */
/* %type <ImportList> import_stmt; */
/* %type <ImportList> dotted_as_names; */
/* %type <Alias> dotted_as_name; */
/* %type <std::string> dotted_name; */

/* %type <StatementList> stmt_list */
/* %type <Statement> stmt; */
/* %type <Statement> assignment_expr; */

/* %type <Expr> expr */
%type <ExprHolder> assignment_expr
%type <ExprHolder> exponential_expr
%type <ExprHolder> unary_expr
%type <ExprHolder> multiplicative_expr
%type <ExprHolder> additive_expr
%type <ExprHolder> relational_expr
%type <ExprHolder> equality_expr
%type <ExprHolder> logical_expr
%type <ExprHolder> expr
%type <Literal> literal

%start program

%%

program
    : stmt_list END
    | import_list stmt_list END
    ;

/* module system */
import_list
    : import_stmt
    | import_list import_stmt
    ;

import_stmt
    : IMPORT dotted_as_names
    | IMPORT LPAREN dotted_as_names RPAREN
    | FROM dotted_name IMPORT import_as_names
    | FROM dotted_name IMPORT LPAREN import_as_names RPAREN
    ;

/* import a.b.c */
dotted_as_names
    : dotted_as_name
    | dotted_as_names COMMA dotted_as_name
    ;

dotted_as_name
    : dotted_name
    | dotted_name AS IDENTIFIER
    ;

/* from a.b import c */
import_as_names
    : import_as_name
    | import_as_names COMMA import_as_name
    ;

import_as_name
    : IDENTIFIER
    | IDENTIFIER AS IDENTIFIER;

dotted_name
    : IDENTIFIER
    | dotted_name '.' IDENTIFIER;
    ;

stmt_list
    : stmt
    | stmt_list stmt
    ;

stmt
    : assignment_expr
    | definition
    | function_call
    | compound_stmt
    | THROW expr
    | RETURN exprlist;

assignment_expr
    : LET IDENTIFIER[NAME] EQ expr[VALUE] {
      $$ = ExprHolder{AssignmentExpr(std::move($NAME), std::move($VALUE))};
    }
    | IDENTIFIER[NAME] EQ expr[VALUE]
    | IDENTIFIER LBRACK atom RBRACK EQ expr
    ;

literal
    : FLOAT   { $$ = Float($1); }
    | INTEGER { $$ = Integer($1); }
    | STRING  { $$ = String($1); }
    ;

atom
    : dotted_name
    | literal
    | LPAREN expr RPAREN
    | LBRACK list_expr RBRACK;

/* expressions */
atom_expr
    : atom
    | atom trailer;

trailer
    : LPAREN RPAREN
    | LPAREN args_list RPAREN
    | LBRACK expr RBRACK;

list_expr
    : atom
    | list_expr COMMA atom;

exponential_expr
    : atom_expr
    | exponential_expr POW atom_expr;

unary_expr
    : exponential_expr
    /* | NOT unary_expr[RHS] */
    /* | MINUS unary_expr[RHS] */
    | NOT unary_expr[RHS]   {
        $$ = ExprHolder{UnaryExpr(UnaryExpr::NOT, std::move($RHS))};
    }
    | MINUS unary_expr[RHS] {
        $$ = ExprHolder{UnaryExpr(UnaryExpr::MINUS, std::move($RHS))};
    }
    ;

multiplicative_expr
    : unary_expr
    /* | multiplicative_expr[LHS] MUL unary_expr[RHS] */
    /* | multiplicative_expr[LHS] DIV unary_expr[RHS] */
    | multiplicative_expr[LHS] MUL unary_expr[RHS] {
        $$ = ExprHolder{MultiplicativeExpr(std::move($LHS), MultiplicativeExpr::MUL, std::move($RHS))};
    }
    | multiplicative_expr[LHS] DIV unary_expr[RHS] {
        $$ = ExprHolder{MultiplicativeExpr(std::move($LHS), MultiplicativeExpr::DIV, std::move($RHS))};
    }
    ;

additive_expr
    : multiplicative_expr
    /* | additive_expr[LHS] PLUS multiplicative_expr[RHS] */
    /* | additive_expr[LHS] MINUS multiplicative_expr[RHS] */
    | additive_expr[LHS] PLUS multiplicative_expr[RHS]  { $$ = ExprHolder{AdditiveExpr(std::move($LHS), AdditiveExpr::PLUS, std::move($RHS))}; };
    | additive_expr[LHS] MINUS multiplicative_expr[RHS] { $$ = ExprHolder{AdditiveExpr(std::move($LHS), AdditiveExpr::MINUS, std::move($RHS))}; };
    ;

relational_expr
    : additive_expr
    /* | relational_expr[LHS] LT additive_expr[RHS] */
    /* | relational_expr[LHS] LE additive_expr[RHS] */
    /* | relational_expr[LHS] GT additive_expr[RHS] */
    /* | relational_expr[LHS] GE additive_expr[RHS] */
    | relational_expr[LHS] LT additive_expr[RHS] { $$ = ExprHolder{RelationalExpr(std::move($LHS), RelationalExpr::LT, std::move($RHS))}; }
    | relational_expr[LHS] LE additive_expr[RHS] { $$ = ExprHolder{RelationalExpr(std::move($LHS), RelationalExpr::LE, std::move($RHS))}; }
    | relational_expr[LHS] GT additive_expr[RHS] { $$ = ExprHolder{RelationalExpr(std::move($LHS), RelationalExpr::GT, std::move($RHS))}; }
    | relational_expr[LHS] GE additive_expr[RHS] { $$ = ExprHolder{RelationalExpr(std::move($LHS), RelationalExpr::GE, std::move($RHS))}; }
    ;

equality_expr
    : relational_expr
    /* | equality_expr[LHS] EQ relational_expr[RHS] */
    /* | equality_expr[LHS] NE relational_expr[RHS] */
    | equality_expr[LHS] EQ relational_expr[RHS] { $$ = ExprHolder{EqualityExpr(std::move($LHS), EqualityExpr::EQ, std::move($RHS))}; }
    | equality_expr[LHS] NE relational_expr[RHS] { $$ = ExprHolder{EqualityExpr(std::move($LHS), EqualityExpr::NE, std::move($RHS))}; }
    ;

logical_expr
    : equality_expr
    /* | logical_expr[LHS] AND equality_expr[RHS] */
    /* | logical_expr[LHS] OR equality_expr[RHS] */
    /* | logical_expr[LHS] XOR equality_expr[RHS] */
    | logical_expr[LHS] AND equality_expr[RHS] { $$ = ExprHolder{LogicalExpr(std::move($LHS), LogicalExpr::AND, std::move($RHS))}; }
    | logical_expr[LHS] OR equality_expr[RHS]  { $$ = ExprHolder{LogicalExpr(std::move($LHS), LogicalExpr::OR, std::move($RHS))}; }
    | logical_expr[LHS] XOR equality_expr[RHS] { $$ = ExprHolder{LogicalExpr(std::move($LHS), LogicalExpr::XOR, std::move($RHS))}; }
    ;

expr
    : logical_expr;

exprlist
    : expr
    | exprlist COMMA expr;

definition
    : function_definition
    | class_definition
    | EXPORT function_definition
    | EXPORT class_definition;

function_definition
    : LET dotted_name LPAREN RPAREN scope
    | LET dotted_name LPAREN function_params_list RPAREN scope;

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
    : INDENT stmt_list DEDENT;

class_definition
    : CLASS IDENTIFIER class_scope;

class_scope
    : INDENT inner_class_scope DEDENT;

inner_class_scope
    : assignment_expr
    | function_definition
    | inner_class_scope assignment_expr
    | inner_class_scope function_definition;

function_call
    : IDENTIFIER LPAREN RPAREN
    | IDENTIFIER LPAREN args_list RPAREN;

args_list
    : expr_list
    | expr_list COMMA variadic_param
    | variadic_param;

expr_list
    : expr
    | expr_list COMMA expr;

/* flow control */
compound_stmt
    : if_stmt
    | while_stmt
    | for_stmt
    | try_stmt;

if_stmt
    : IF expr scope
    | IF expr scope else_stmt;

else_stmt
    : ELSE if_stmt
    | ELSE scope;

while_stmt
    : WHILE expr scope;

for_stmt
    : FOR exprlist IN expr scope;

try_stmt
    : TRY scope CATCH dotted_as_name scope;

%%

void yy::parser::error(const yy::location& loc, const std::string& message)
{
    std::cout << "Error: " << message << "\nLocation: L" << loc.end.line
        << " C" << loc.end.column << '\n';
}

/* vim: set ft=yacc: */
