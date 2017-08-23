%token NEWLINE INDENT DEDENT
%token INTEGER_LITERAL FLOAT_LITERAL STRING_LITERAL
%token NOT POW MUL DIV ADD SUB LT LE GT GE EQ NE AND OR XOR
%token AS CLASS ELSE EXPORT FOR FROM IF IMPORT IN LET RETURN THROW WHILE
%token IDENTIFIER ELLIPSIS

%start code

%%

code
    : preamble program
    | program;

literal
    : INTEGER_LITERAL
    | FLOAT_LITERAL
    | STRING_LITERAL;


condition
    : operator;

operator
    : IDENTIFIER
    | IDENTIFIER '[' operator ']'
    | literal
    | '(' logical_expr ')';

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

preamble
    : preamble imports
    | imports;

imports
    : module_import
    | feature_import;

module_import
    : IMPORT IDENTIFIER
    | IMPORT IDENTIFIER module_aliasing;

feature_import
    : FROM IDENTIFIER IMPORT feature_name_list;

feature_name_list
    : feature_name
    | feature_name_list ',' feature_name;

feature_name
    : IDENTIFIER
    | IDENTIFIER module_aliasing;

module_aliasing
    : AS IDENTIFIER;


program
    : command
    | definition
    | command program
    | definition program;

definition
    : function_definition
    | class_definition
    | EXPORT function_definition
    | EXPORT class_definition;


function_definition
    : LET IDENTIFIER '(' function_params_list ')' scope
    | LET IDENTIFIER '(' ')' scope;

function_params_list
    : identifier_list
    | identifier_list ',' variadic_param
    | variadic_param;

identifier_list
    : IDENTIFIER
    | identifier_list ',' IDENTIFIER
    | IDENTIFIER '=' operator
    | identifier_list ',' IDENTIFIER '=' operator;

variadic_param
    : ELLIPSIS IDENTIFIER;

scope
    : LINE_BREAK INDENT inner_scope DEDENT;

inner_scope
    : command
    | inner_scope command;


class_definition
    : CLASS IDENTIFIER class_scope;

class_scope
    : LINE_BREAK INDENT inner_class_scope DEDENT;

inner_class_scope
    : attr_decl
    | function_definition
    | inner_class_scope attr_decl
    | inner_class_scope function_definition;

attr_decl
    : LET IDENTIFIER '=' operator LINE_BREAK
    | LET IDENTIFIER LINE_BREAK;


command
    : assignment_expr LINE_BREAK
    | function_call LINE_BREAK
    | statement LINE_BREAK
    | THROW operator LINE_BREAK
    | RETURN operator LINE_BREAK;

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

statement
    : if_statement
    | while_statement
    | for_statement;

if_statement
    : IF condition scope
    | IF condition scope ELSE scope;

while_statement
    : WHILE condition scope;

for_statement
    : FOR IDENTIFIER IN operator scope;

%%

#include <stdio.h>

extern char yytext[];
extern int column;

int yyerror(char *s)
{
    fflush(stdout);
    printf("\n%*s\n%*s\n", column, "^", column, s);
}
