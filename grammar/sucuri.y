%require "3.0"

%language "c++"
%skeleton "lalr1.cc"

%token INDENT DEDENT
%token AS CATCH CLASS ELSE EXPORT FOR FROM IF IMPORT IN LET RETURN THROW TRY WHILE
%token ELLIPSIS

%token END 0
%token COMMA ","
%token LPAREN "("
%token RPAREN ")"
%token LBRACK "["
%token RBRACK "]"

%left NOT "not"
%left POW "**"
%left MUL "*"
%left DIV "/"
%left PLUS "+"
%left MINUS "-"
%left LT "<"
%left LE "<="
%left GT ">"
%left GE ">="
%left EQ "="
%left NE "!="
%left AND "and"
%left OR "or"
%left XOR "xor"

%verbose

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.error verbose
%define parse.trace

%token <long double> FLOAT
%token <long long> INTEGER
%token <bool> BOOL
%token <std::string> STRING
%token <std::string> IDENTIFIER

%locations
%param {parser::semantic_type& yylval} {parser::location_type& yylloc}

%code requires {
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/raw_ostream.h>

#include <sstream>
#include <utility>

#include "utils.h"

using namespace utils;

using namespace std::string_literals;

static llvm::LLVMContext context{};
static auto module = llvm::make_unique<llvm::Module>("test-module", context);
static std::vector<llvm::BasicBlock*> blocks{};
static std::vector<llvm::Function*> functions{};

inline auto current_block() {
    return blocks.back();
}

inline auto current_function() {
    return functions.back();
}

}

%code {

yy::parser::symbol_type yylex(
    yy::parser::semantic_type& yylval,
    yy::parser::location_type& yylloc
);

}

/* %type <Node> atom_expr */

%type <llvm::Constant*> literal
%type <llvm::Value*> atom_expr
%type <llvm::Value*> unary_expr
%type <llvm::Value*> multiplicative_expr
%type <llvm::Value*> additive_expr
%type <llvm::Value*> relational_expr
%type <llvm::Value*> equality_expr
%type <llvm::Value*> logical_expr
%type <llvm::Value*> expr
%type <std::string> dotted_name

%start program

%%

program
    :
    {
        std::cout << std::string(80, '-') << "\nCompiling...\n";
    }
    code END
    {
        std::cout << std::string(80, '-') << "\nDone.\n";
        module->print(llvm::errs(), nullptr);
    }
    ;

code
    :
    {
        std::cout << "stmt_list\n";

        auto types = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context),
            false
        );

        functions.emplace_back(
            llvm::Function::Create(
                types,
                llvm::GlobalVariable::LinkageTypes::ExternalLinkage,
                module->getName() + "::main",
                module.get()
            )
        );

        blocks.emplace_back(
            llvm::BasicBlock::Create(context, "first", current_function())
        );
    }
    stmt_list
    {
        blocks.pop_back();
        functions.pop_back();
    }
    | import_list stmt_list
    ;

identifier
    : IDENTIFIER
    ;

/* module system */
import_list
    : import_stmt
    | import_list import_stmt
    ;

import_stmt
    : IMPORT dotted_as_names[NAMES]
    | IMPORT LPAREN dotted_as_names[NAMES] RPAREN
    | FROM dotted_name[PATH] IMPORT import_as_names[NAMES]
    | FROM dotted_name[PATH] IMPORT LPAREN import_as_names[NAMES] RPAREN
    ;

/* import a.b.c */
dotted_as_names
    : dotted_as_name
    | dotted_as_names[L] COMMA dotted_as_name[NAME]
    ;

dotted_as_name
    : dotted_name
    | dotted_name AS identifier
    ;

/* from a.b import c */
import_as_names
    : import_as_name
    | import_as_names COMMA import_as_name
    ;

import_as_name
    : identifier
    | identifier AS identifier
    ;

dotted_name
    : identifier
    | dotted_name '.' identifier
    ;

stmt_list
    : stmt
    | stmt_list[LIST] stmt
    ;

stmt
    : variable_declaration
    |
    {
        std::cout << "assignment_expr\n";
    }
    assignment_expr
    /*| definition*/
    /*| function_call*/
    | compound_stmt
    | THROW expr
    | RETURN expr[EXPR]
    ;

variable_declaration
    :
    {
        std::cout << "variable_declaration\n";
    }
    LET IDENTIFIER[NAME] EQ expr[VALUE]
    {
        auto layout = llvm::DataLayout(module.get());
        std::cout << $NAME << ".size: "
                  << layout.getTypeStoreSize($VALUE->getType()) << "\n";
        std::cout << $NAME << ".value: " << $VALUE << "\n";
        auto builder = llvm::IRBuilder<>{current_block()};
        auto alloca = builder.CreateAlloca($VALUE->getType(), nullptr, $NAME);
        auto store = builder.CreateStore($VALUE, alloca);
    }
    ;

assignment_expr
    :
    dotted_name[NAME] EQ expr[VALUE]
    ;

literal
    : FLOAT
    |
    {
        std::cout << "integer literal\n";
    }
    INTEGER
    {
        $$ = llvm::ConstantInt::get(
            module->getContext(),
            llvm::APInt(32, $INTEGER, true)
        );
        std::cout << "\t>>> value: " << $INTEGER << "\n";
    }
    | STRING
    | BOOL
    ;

atom
    : dotted_name
    |
    literal
    | LPAREN expr RPAREN
    | LBRACK list_expr RBRACK
    ;

/* expressions */
atom_expr
    : atom
    | atom trailer
    ;

trailer
    : LPAREN RPAREN
    | LBRACK expr RBRACK
    ;

list_expr
    : atom
    | list_expr COMMA atom
    ;

exponential_expr
    : atom_expr
    | exponential_expr[LHS] POW atom_expr[RHS]
    ;

unary_expr
    : exponential_expr
    | NOT unary_expr[RHS]
    | MINUS unary_expr[RHS]
    ;

multiplicative_expr
    : unary_expr
    {
        $$ = std::move($1);
    }
    | multiplicative_expr[LHS] MUL unary_expr[RHS]
    | multiplicative_expr[LHS] DIV unary_expr[RHS]
    {
        std::cout << "multiplicative_expr (DIV)\n";
        llvm::IRBuilder<> builder{current_block()};
        $$ = std::move(builder.CreateFDiv($LHS, $RHS));
        std::cout << "\t>>> div: " << $$ << "\n";
    }
    ;

additive_expr
    : multiplicative_expr
    {
        std::cout << "(PASS-ON) additive_expr\n";
        $$ = std::move($1);
    }
    | additive_expr[LHS] PLUS multiplicative_expr[RHS]
    {
        std::cout << "additive_expr\n";
    }
    | additive_expr[LHS] MINUS multiplicative_expr[RHS]
    ;

relational_expr
    : additive_expr
    {
        std::cout << "(PASS-ON) relational_expr\n";
        $$ = std::move($1);
    }
    | relational_expr[LHS] LT additive_expr[RHS]
    | relational_expr[LHS] LE additive_expr[RHS]
    | relational_expr[LHS] GT additive_expr[RHS]
    | relational_expr[LHS] GE additive_expr[RHS]
    ;

equality_expr
    : relational_expr
    {
        std::cout << "(PASS-ON) equality_expr\n";
        $$ = std::move($1);
    }
    | equality_expr[LHS] EQ relational_expr[RHS]
    | equality_expr[LHS] NE relational_expr[RHS]
    ;

logical_expr
    : equality_expr
    {
        std::cout << "(PASS-ON) logical_expr\n";
        $$ = std::move($1);
    }
    | logical_expr[LHS] AND equality_expr[RHS]
    | logical_expr[LHS] OR equality_expr[RHS]
    | logical_expr[LHS] XOR equality_expr[RHS]
    ;

expr
    : logical_expr[EXPR]
    {
        std::cout << "(PASS-ON) expr\n";
        $$ = std::move($EXPR);
    }
    ;

scope
    : INDENT stmt_list DEDENT
    ;

/* flow control */
compound_stmt
    : if_stmt
    ;

if_stmt
    : IF expr scope
    | IF expr scope else_stmt
    ;

else_stmt
    : ELSE if_stmt
    | ELSE scope
    ;

%%

void yy::parser::error(const yy::location& loc, const std::string& message)
{
    static const auto red = "\033[31m";
    static const auto bold = "\033[1m";
    static const auto reset = "\033[0m";

    auto filename = "stdin"s;
    if (loc.end.filename) {
        filename = *loc.end.filename;
    }

    std::ostringstream os;
    os << loc.end.line << ":" << loc.end.column;
    const auto lc = os.str();

    std::cerr << bold << filename << ":" << lc << ": "
              << red << "error: " << reset
              << bold << message << reset << '\n';
}

/* vim: set ft=yacc: */
