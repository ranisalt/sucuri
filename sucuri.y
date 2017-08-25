%token NEWLINE INDENT DEDENT
%token INTEGER_LITERAL FLOAT_LITERAL STRING_LITERAL
%token NOT POW MUL DIV PLUS MINUS LT LE GT GE EQ NE AND OR XOR
%token AS CATCH CLASS ELSE EXPORT FOR FROM IF IMPORT IN LET RETURN THROW TRY WHILE
%token IDENTIFIER ELLIPSIS

%start code

%%

code
    : program
    | imports program;

atom
    : IDENTIFIER
    | INTEGER_LITERAL
    | FLOAT_LITERAL
    | STRING_LITERAL
    | '(' expr ')'
    | '[' expr ']';

/* expressions */
atom_expr
    : atom
    | atom trailer;

trailer
    : '(' ')'
    | '(' arglist ')'
    | '[' expr ']'
    | '.' IDENTIFIER;

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
    | exprlist ',' expr;

/* module system */
imports
    : import_stmt NEWLINE
    | imports import_stmt NEWLINE;

import_stmt
    : IMPORT dotted_as_names
    | IMPORT dotted_as_names ','
    | FROM dotted_name IMPORT import_as_names
    | FROM dotted_name IMPORT import_as_names ',';

/* import a.b.c */
dotted_as_names
    : dotted_as_name
    | dotted_as_names ',' dotted_as_name;

dotted_as_name
    : dotted_name
    | dotted_name AS IDENTIFIER;

/* from a.b import c */
import_as_names
    : import_as_name
    | import_as_names ',' import_as_name;

import_as_name
    : IDENTIFIER
    | IDENTIFIER AS IDENTIFIER;

dotted_name
    : IDENTIFIER
    | dotted_name '.' IDENTIFIER;

program
    : stmt
    | definition
    | stmt program
    | definition program;

definition
    : function_definition
    | class_definition
    | EXPORT function_definition
    | EXPORT class_definition;


function_definition
    : LET IDENTIFIER '(' ')' scope
    | LET IDENTIFIER '(' function_params_list ')' scope;

function_params_list
    : identifier_list
    | identifier_list ',' variadic_param
    | variadic_param;

identifier_list
    : IDENTIFIER
    | IDENTIFIER EQ atom
    | identifier_list ',' IDENTIFIER
    | identifier_list ',' IDENTIFIER EQ atom;

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
    : LET IDENTIFIER EQ atom
    | IDENTIFIER EQ atom
    | IDENTIFIER '[' atom ']' EQ atom;

function_call
    : IDENTIFIER '(' ')'
    | IDENTIFIER '(' arglist ')';

arglist
    : atom
    | arglist ',' atom;

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

#include <stdio.h>

extern char yytext[];
extern int column;

int yyerror(char *s)
{
    fflush(stdout);
    printf("\n%*s\n%*s\n", column, "^", column, s);
}
