%token NEWLINE INDENT DEDENT
%token INTEGER_LITERAL FLOAT_LITERAL STRING_LITERAL
%token NOT POW MUL DIV ADD SUB LT LE GT GE EQ NE AND OR XOR
%token AS CLASS ELSE EXPORT FOR FROM IF IMPORT IN LET RETURN THROW WHILE
%token IDENTIFIER ELLIPSIS

%start code

%%

code
    : program
    | imports program;

literal
    : INTEGER_LITERAL
    | FLOAT_LITERAL
    | STRING_LITERAL;

operator
    : IDENTIFIER
    | IDENTIFIER '[' operator ']'
    | literal
    | '(' logical_expr ')';

/* expressions */
unary_expr
    : NOT operator
    | SUB operator
    | operator;

exponential_expr
    : unary_expr
    | exponential_expr POW unary_expr;

multiplicative_expr
    : exponential_expr
    | multiplicative_expr MUL exponential_expr
    | multiplicative_expr DIV exponential_expr;

additive_expr
    : multiplicative_expr
    | additive_expr ADD multiplicative_expr
    | additive_expr SUB multiplicative_expr;

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

exprlist
    : logical_expr
    | exprlist ',' logical_expr;

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
    | IDENTIFIER EQ operator
    | identifier_list ',' IDENTIFIER
    | identifier_list ',' IDENTIFIER EQ operator;

variadic_param
    : ELLIPSIS IDENTIFIER;

scope
    : NEWLINE INDENT inner_scope DEDENT;

inner_scope
    : stmt
    | inner_scope stmt;


class_definition
    : CLASS IDENTIFIER class_scope;

class_scope
    : NEWLINE INDENT inner_class_scope DEDENT;

inner_class_scope
    : function_definition
    | inner_class_scope function_definition;

stmt
    : assignment_expr NEWLINE
    | function_call NEWLINE
    | compound_stmt NEWLINE
    | THROW operator NEWLINE
    | RETURN operator NEWLINE;

assignment_expr
    : LET IDENTIFIER EQ operator
    | IDENTIFIER EQ operator
    | IDENTIFIER '[' operator ']' EQ operator;

function_call
    : IDENTIFIER '(' ')'
    | IDENTIFIER '(' function_args ')';

function_args
    : operator
    | function_args ',' operator;

/* flow control */
compound_stmt
    : if_stmt
    | while_stmt
    | for_stmt;

if_stmt
    : IF logical_expr scope
    | IF logical_expr scope ELSE scope;

while_stmt
    : WHILE logical_expr scope;

for_stmt
    : FOR exprlist IN logical_expr scope;

%%

#include <stdio.h>

extern char yytext[];
extern int column;

int yyerror(char *s)
{
    fflush(stdout);
    printf("\n%*s\n%*s\n", column, "^", column, s);
}
