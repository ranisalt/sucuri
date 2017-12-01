#pragma once

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <memory>
#include <vector>
#include <string>

namespace llvm {
class Value;
}

namespace AST {

class Node
{
public:
    constexpr Node() = default;
    virtual ~Node() = default;

    Node(const Node& other) = default;

    template<class T>
        Node(T data):
            ptr{std::make_shared<model<T>>(std::move(data))}
    {}

    Node& operator=(const Node& rhs) = default;

    template<class T>
    Node& operator=(T data) {
        ptr = std::make_shared<model<T>>(std::move(data));
        return *this;
    }

    virtual llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const {
        std::cout << "Node::to_llvm(): " << ptr << "\n";
        if (ptr) {
            auto hehe = std::make_shared<concept>(*ptr);
            hehe->block = block;
            return hehe->to_llvm(builder, context);
        }
        return nullptr;
    }

    virtual std::string to_string() const {
        return ptr ? ptr->to_string() : "ptr null";
    }

    template<class T>
    T* as() {
        return dynamic_cast<T*>(ptr.get());
    }

    template<class T>
    const T* as() const {
        return dynamic_cast<T*>(ptr.get());
    }

    llvm::BasicBlock* block = nullptr;

private:
public:
    struct concept {
        virtual ~concept() = default;
        virtual std::unique_ptr<concept> copy() const = 0;
        virtual llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const = 0;
        virtual std::string to_string() const = 0;
        llvm::BasicBlock* block = nullptr;
    };

    template <class T>
    struct model final: concept {
        model(T data):
            data{std::move(data)}
        {}

        std::unique_ptr<concept> copy() const {
            return std::make_unique<model<T>>(data);
        }
        llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const {
            auto data = this->data;
            data.block = block;
            return data.to_llvm(builder, context);
        }

        std::string to_string() const {
            return data.to_string();
        }

        llvm::BasicBlock* block = nullptr;

        T data;
    };

    std::shared_ptr<concept> ptr;
};


template <>
struct Node::model<std::vector<Node>> final: concept {
    using T = std::vector<Node>;

    model(T data):
        data{std::move(data)}
    {}

    std::unique_ptr<concept> copy() const {
        return std::make_unique<model<T>>(data);
    }

    llvm::Value* to_llvm() const {
        std::cout << "Node::model::to_llvm() (for " << to_string() << ")\n";
        return nullptr;
    }
    std::string to_string() const {
        return "ah vsf\n";
    }

    T data;
};


struct Name
{
    Name() = default;

    Name(std::vector<std::string> path): path{std::move(path)} {}

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    void append(std::string i) {
        path.push_back(std::move(i));
    }

    std::vector<std::string> path;
    llvm::BasicBlock* block = nullptr;
};

struct Alias {
    Alias() = default;

    Alias(Name name):
        alias{name, name.path[0]}
    {}

    Alias(std::pair<Name, std::string> alias):
        alias{alias}
    {}

    std::string to_string() const;

    std::pair<Name, std::string> alias;
    llvm::BasicBlock* block = nullptr;
};

inline bool operator==(const Name lhs, const Name rhs) {
    return lhs.to_string() == rhs.to_string();
}

struct Float
{
    Float(double value): value{value} {
        std::cout << to_string() << std::endl;
    }

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    double value;
    llvm::BasicBlock* block = nullptr;
};

struct Integer
{
    Integer(long long value): value{value} {
        std::cout << to_string() << std::endl;
    }

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    long long value;
    llvm::BasicBlock* block = nullptr;
};

struct Bool
{
    Bool(bool value): value{value} {
        std::cout << to_string() << std::endl;
    }

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    bool value;
    llvm::BasicBlock* block = nullptr;
};

struct String
{
    String(std::string value): value{std::move(value)} {
        std::cout << to_string() << std::endl;
    }

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    std::string value;
    llvm::BasicBlock* block = nullptr;
};

struct Literal: public Node
{
    Literal() = default;

    /* explicit Literal(Float value): Node{std::move(value)} {} */
    Literal& operator=(Float value) {
        Node::operator=(std::move(value)); return *this;
    }

    /* explicit Literal(Integer value): Node{std::move(value)} {} */
    Literal& operator=(Integer value) {
        Node::operator=(std::move(value)); return *this;
    }

    /* explicit Literal(String value): Node{std::move(value)} {} */
    Literal& operator=(String value) {
        Node::operator=(std::move(value)); return *this;
    }
};

struct AssignmentExpr
{
    AssignmentExpr() = default;
    AssignmentExpr(
            Name name,
            Node value
            ):
        name{std::move(name)},
        value{std::move(value)}
    {
        std::cout << to_string() << std::endl;
    }

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    Name name;
    Node value;
    llvm::BasicBlock* block = nullptr;
};

struct UnaryExpr
{
    enum Operator {
        NOT, NEG,
    };

    UnaryExpr() = default;

    UnaryExpr(Operator op, Node rhs):
        op{op}, rhs{std::move(rhs)} {
            std::cout << to_string() << std::endl;
        }

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    Operator op;
    Node rhs;
    llvm::BasicBlock* block = nullptr;
};

struct LogicalExpr
{
    enum Operator {
        AND, OR, XOR,
    };

    LogicalExpr(Node lhs, Operator op, Node rhs):
        lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {
            std::cout << to_string() << std::endl;
        }

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    Node lhs;
    Operator op;
    Node rhs;
    llvm::BasicBlock* block = nullptr;
};

struct EqualityExpr
{
    enum Operator {
        EQ, NE,
    };

    EqualityExpr(Node lhs, Operator op, Node rhs):
        lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {
            std::cout << to_string() << std::endl;
        }

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    Node lhs;
    Operator op;
    Node rhs;
    llvm::BasicBlock* block = nullptr;
};

struct RelationalExpr
{
    enum Operator {
        LT, LE, GT, GE,
    };

    RelationalExpr(Node lhs, Operator op, Node rhs):
        lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {
            std::cout << to_string() << std::endl;
        }

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    Node lhs;
    Operator op;
    Node rhs;
    llvm::BasicBlock* block = nullptr;
};

struct AdditiveExpr
{
    enum Operator {
        PLUS, MINUS,
    };

    AdditiveExpr(Node lhs, Operator op, Node rhs):
        lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {
            std::cout << to_string() << std::endl;
        }

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    Node lhs;
    Operator op;
    Node rhs;
    llvm::BasicBlock* block = nullptr;
};

struct MultiplicativeExpr
{
    enum Operator {
        MUL, DIV,
    };

    MultiplicativeExpr() = default;

    MultiplicativeExpr(Node lhs, Operator op, Node rhs):
        lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {
            std::cout << to_string() << std::endl;
        }

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    Node lhs;
    Operator op;
    Node rhs;
    llvm::BasicBlock* block = nullptr;
};

struct ExponentialExpr
{
    ExponentialExpr(Node lhs, Node rhs):
        lhs{std::move(lhs)}, rhs{std::move(rhs)} {
            std::cout << to_string() << std::endl;
        }

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    Node lhs;
    Node rhs;
    llvm::BasicBlock* block = nullptr;
};

struct FunctionDecl
{
  FunctionDecl(std::string name, std::vector<Node> body):
    name{std::move(name)}, body{std::move(body)} { std::cout << to_string() << std::endl; }

  llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

  std::string to_string() const;

  std::string name;
  std::vector<Node> body;
};

struct VariableDecl
{
    VariableDecl() = default;

    VariableDecl(Node expr): expr{std::move(expr)} { std::cout << to_string() << std::endl; }

<<<<<<< Updated upstream
  llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    Node expr;
    llvm::BasicBlock* block = nullptr;
};

struct ListExpr
{
    ListExpr() = default;

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    std::vector<Node> values;
    llvm::BasicBlock* block = nullptr;
};

struct FunctionCall
{
    FunctionCall() = default;

    FunctionCall(Name name, std::vector<Node> expr_list = {}):
        name{std::move(name)}, expr_list(std::move(expr_list)) {
            std::cout << to_string() << std::endl;
        }

    llvm::Value* to_llvm() const;

    std::string to_string() const;

    Name name;
    std::vector<Node> expr_list;
    llvm::BasicBlock* block = nullptr;
};

struct FunctionDefinition
{
    FunctionDefinition() = default;
    FunctionDefinition(Node body, Name name, std::vector<Name> params = {}):
        name{std::move(name)},
        params{std::move(params)},
        body{std::make_shared<Node>(std::move(body))}
    {}

    llvm::Value* to_llvm() const;
    std::string to_string() const;

    Name name;
    std::vector<Name> params;
    std::shared_ptr<Node> body;
    llvm::BasicBlock* block = nullptr;
};

struct Statement: public Node
{
    Statement() = default;

    Statement(Node state):
        state{state}
    {}

    virtual llvm::Value* to_llvm() const;
    virtual std::string to_string() const;

    Node state;
    llvm::BasicBlock* block = nullptr;
};

struct ReturnStatement
{
    ReturnStatement() = default;

    ReturnStatement(Node expr):
        expr{expr}
    {}

    llvm::Value* to_llvm() const;
    std::string to_string() const;

    Node expr;
    llvm::BasicBlock* block = nullptr;
};

struct StatementList
{
    StatementList() = default;

    StatementList(std::vector<Statement> statements):
        statements{std::move(statements)}
    {}

    llvm::Value* to_llvm() const;

    std::string to_string() const;

    void append(Statement node) {
        statements.push_back(std::move(node));
    }

    llvm::Value* to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;
    std::vector<Statement> statements;
    llvm::BasicBlock* block = nullptr;
};

struct Code
{
    Code() = default;

    Code(AST::Node root):
        root{std::move(root)}
    {}

    llvm::Value* to_llvm() const;

    std::string to_string() const;

    AST::Node root;
    llvm::BasicBlock* block = nullptr;
};

struct Program
{
    Program() = default;

    Program(AST::Node root):
        root{std::move(root)}
    {}

    std::unique_ptr<llvm::Module> to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const;

    std::string to_string() const;

    AST::Node root;
};

}
