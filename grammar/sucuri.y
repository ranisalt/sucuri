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


struct Function {
    std::string name = module->getName();
    llvm::Type* return_type = nullptr;
    std::vector<llvm::Type*> param_types = {};
    std::vector<llvm::Value*> params = {};
    llvm::Function* f = nullptr;

    Function() = default;

    Function(
        llvm::Type* return_type,
        std::vector<llvm::Type*> param_types = {},
        std::vector<llvm::Value*> params = {},
        llvm::Function* f = nullptr
    ):
        return_type{return_type},
        param_types{param_types},
        params{params},
        f{f}
    {}

    void make_f() {
        f = llvm::Function::Create(
                llvm::FunctionType::get(
                    return_type,
                    param_types,
                    false
                ),
                llvm::GlobalVariable::LinkageTypes::ExternalLinkage,
                name,
                module.get()
            );
    }
};

static std::vector<Function> functions{};

inline auto& current_block() {
    return blocks.back();
}

inline auto& current_function() {
    return functions.back();
}


inline auto to_type(const std::string& type) -> llvm::Type* {
    if (type == "int") {
        return llvm::Type::getInt32Ty(context);
    } else if (type == "float") {
        return llvm::Type::getDoubleTy(context);
    } else if (type == "let") {
        return llvm::Type::getDoubleTy(context);
    }
    return nullptr;
}

}

%code {

yy::parser::symbol_type yylex(
    yy::parser::semantic_type& yylval,
    yy::parser::location_type& yylloc
);

}

%type <llvm::Constant*> literal
%type <llvm::Value*> atom
%type <llvm::Value*> atom_expr
%type <llvm::Value*> unary_expr
%type <llvm::Value*> exponential_expr
%type <llvm::Value*> multiplicative_expr
%type <llvm::Value*> additive_expr
%type <llvm::Value*> relational_expr
%type <llvm::Value*> equality_expr
%type <llvm::Value*> logical_expr
%type <llvm::Value*> expr
%type <std::string> dotted_name

%type <llvm::Value*> return_stmt

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

        functions.emplace_back(
            Function(
                llvm::Type::getInt32Ty(context)
            )
        );
        current_function().make_f();

        blocks.emplace_back(
            llvm::BasicBlock::Create(context, "first", current_function().f)
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
    :
    definition
    |
    {
        std::cout << "assignment_expr\n";
    }
    assignment_expr
    /*| function_call*/
    | compound_stmt
    | THROW expr
    |
    {
        std::cout << "return stmt\n";
    }
    return_stmt
    ;

definition
    :
    variable_decl
    |
    function_decl
    ;

variable_decl
    :
    LET IDENTIFIER[NAME] EQ expr[VALUE]
    {
        std::cout << "variable_decl: " << $NAME << "\n";
        auto builder = llvm::IRBuilder<>{current_block()};
        auto alloca = builder.CreateAlloca($VALUE->getType(), nullptr, $NAME);
        builder.CreateStore($VALUE, alloca);
    }
    ;

function_decl
    :
    LET IDENTIFIER[NAME] LPAREN IDENTIFIER[TYPE] IDENTIFIER[PARAM]
    {
        std::cout << "\nfunction_decl\n";
        auto f = Function{};

        std::cout << "    param: " << $PARAM << " (" << $TYPE << ")" << std::endl;

        // generate name
        auto name = std::string{};
        auto foo = false;
        for (const auto& function: functions) {
            if (foo) {
                name += "::";
            } else {
                foo = true;
            }
            name += function.f->getName().str();
        }
        name += "::" + $NAME;
        f.name = name;

        // generate types
        auto return_type = to_type("let");
        auto param_type = to_type($TYPE);

        f.return_type = return_type;
        f.param_types.push_back(param_type);

        f.make_f();

        std::cout << "    params: " << f.params.size() << std::endl;

        auto& param = *std::begin(f.f->args());

        param.setName($PARAM);

        for (auto& param: f.f->args()) {
            std::cout << "    |----- " << param.getName().str() << std::endl;
            f.params.push_back(&param);
        }

        functions.emplace_back(f);
    }
    RPAREN
    {
        blocks.emplace_back(llvm::BasicBlock::Create(context, "scope-1"));
    }
    scope
    {
        auto& f = current_function();

        std::cout << "function_decl: " << f.name << "\n";

        const auto& last = current_block()->back();
        auto& param_types = f.param_types;
        auto types = llvm::FunctionType::get(
            last.getType(),
            param_types,
            false
        );

        f.return_type = last.getType();
        current_block()->insertInto(f.f);
        blocks.pop_back();
    }
    ;

assignment_expr
    :
    dotted_name[NAME] EQ expr[VALUE]
    ;

literal
    :
    FLOAT
    {
        auto value = double($FLOAT);
        $$ = llvm::ConstantFP::get(
            module->getContext(),
            llvm::APFloat(value)
        );
    }
    |
    INTEGER
    {
        $$ = llvm::ConstantInt::get(
            module->getContext(),
            llvm::APInt(32, $INTEGER, true)
        );
    }
    | STRING
    | BOOL
    ;

atom
    :
    IDENTIFIER
    {
        std::cout << "IDENTIFIER: " << $1 << std::endl;
        auto found = false;
        for (const auto& param: current_function().params) {
            if (param->getName() == $1) {
                $$ = param;
                found = true;
                break;
            }
        }
        if (not found) {
            error(yylloc, "|  not found");
        }
    }
    |
    literal
    {
        $$ = std::move($literal);
    }
    /*
    | LPAREN expr RPAREN
    */
    | LBRACK list_expr RBRACK
    ;

/* expressions */
atom_expr
    : atom
    {
        $$ = std::move($1);
    }
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
    {
        $$ = std::move($1);
    }
    | exponential_expr[LHS] POW atom_expr[RHS]
    ;

unary_expr
    : exponential_expr
    {
        $$ = std::move($1);
    }
    | NOT unary_expr[RHS]
    | MINUS unary_expr[RHS]
    ;

multiplicative_expr
    : unary_expr
    {
        $$ = std::move($1);
    }
    | multiplicative_expr[LHS] MUL unary_expr[RHS]
    {
        llvm::IRBuilder<> builder{current_block()};

        auto is_float =
            $LHS->getType()->isFloatingPointTy() or
            $RHS->getType()->isFloatingPointTy();

        if (is_float) {
            $$ = std::move(builder.CreateFMul($LHS, $RHS));
        } else {
            $$ = std::move(builder.CreateMul($LHS, $RHS));
        }
    }
    | multiplicative_expr[LHS] DIV unary_expr[RHS]
    {
        llvm::IRBuilder<> builder{current_block()};

        auto is_float =
            $LHS->getType()->isFloatingPointTy() or
            $RHS->getType()->isFloatingPointTy();

        if (is_float) {
            $$ = std::move(builder.CreateFDiv($LHS, $RHS));
        } else {
            $$ = std::move(builder.CreateSDiv($LHS, $RHS));
        }
    }
    ;

additive_expr
    : multiplicative_expr
    {
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
        $$ = std::move($1);
    }
    | equality_expr[LHS] EQ relational_expr[RHS]
    | equality_expr[LHS] NE relational_expr[RHS]
    ;

logical_expr
    : equality_expr
    {
        $$ = std::move($1);
    }
    | logical_expr[LHS] AND equality_expr[RHS]
    | logical_expr[LHS] OR equality_expr[RHS]
    | logical_expr[LHS] XOR equality_expr[RHS]
    ;

expr
    :
    logical_expr[EXPR]
    {
        $$ = std::move($EXPR);
    }
    ;

scope
    :
    {
        std::cout << "Scope\n";
    }
    INDENT stmt DEDENT
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

return_stmt
    :
    RETURN
    {
        std::cout << "return (void)\n";
        auto builder = llvm::IRBuilder<>{current_block()};
        $$ = builder.CreateRetVoid();
    }
    |
    RETURN expr[EXPR]
    {
        std::cout << "return expr" << std::endl;
        auto builder = llvm::IRBuilder<>{current_block()};
        $$ = builder.CreateRet($EXPR);
    }
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
