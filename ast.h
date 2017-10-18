#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <string>

namespace AST {

struct Node
{
  Node() = default;
  virtual ~Node() = default;

  virtual std::string to_string() const { return "Not implemented."; }
};

struct Expr: public Node
{
  /* virtual ~Expr() = default; */
};

class ExprHolder
{
  public:
    constexpr ExprHolder() = default;

    ExprHolder(const ExprHolder& other):
      ptr{other.ptr ? other.ptr->copy() : nullptr} {}

    template<class T>
    ExprHolder(T data):
      ptr{std::make_unique<model<T>>(std::move(data))} {}

    ExprHolder& operator=(const ExprHolder& rhs)
    { ptr = rhs.ptr ? rhs.ptr->copy() : nullptr; return *this; }

    std::string to_string() const { return ptr ? ptr->to_string() : ""; }

  private:
    struct concept {
      virtual ~concept() = default;
      virtual std::unique_ptr<concept> copy() const = 0;
      virtual std::string to_string() const = 0;
    };

    template<class T>
    struct model final: concept {
      model(T data): data{std::move(data)} {}
      std::unique_ptr<concept> copy() const override { return std::make_unique<model<T>>(data); }
      std::string to_string() const override { return data.to_string(); }

      T data;
    };

    std::unique_ptr<concept> ptr;
};

struct AssignmentExpr: public Expr
{
  AssignmentExpr(std::string identifier, ExprHolder value):
    identifier{std::move(identifier)}, value{std::move(value)} { std::cout << to_string() << std::endl; }

  std::string to_string() const override;

  std::string identifier;
  ExprHolder value;
};

struct Literal: public Expr {};

struct Float: public Literal
{
  Float(long double value): value{value} {}

  std::string to_string() const override;

  long double value;
};

struct Integer: public Literal
{
  Integer(long long value): value{value} {}

  std::string to_string() const override;

  long long value;
};

struct String: public Literal
{
  String(std::string value): value{std::move(value)} {}

  std::string to_string() const override;

  std::string value;
};

struct UnaryExpr: public Expr
{
  enum Operator {
    NOT, MINUS,
  };

  UnaryExpr() = default;

  UnaryExpr(Operator op, ExprHolder rhs):
    op{op}, rhs{std::move(rhs)} {}

  std::string to_string() const override;

  Operator op;
  ExprHolder rhs;
};

struct LogicalExpr: public Expr
{
  enum Operator {
    AND, OR, XOR,
  };

  LogicalExpr(ExprHolder lhs, Operator op, ExprHolder rhs):
    lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {}

  ~LogicalExpr() = default;

  ExprHolder lhs;
  Operator op;
  ExprHolder rhs;
};

struct EqualityExpr: public Expr
{
  public:
    enum Operator {
      EQ, NE,
    };

    EqualityExpr(ExprHolder lhs, Operator op, ExprHolder rhs):
      lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {}

    ~EqualityExpr() = default;

    ExprHolder lhs;
    Operator op;
    ExprHolder rhs;
};

struct RelationalExpr: public Expr
{
  public:
    enum Operator {
      LT, LE, GT, GE,
    };

    RelationalExpr(ExprHolder lhs, Operator op, ExprHolder rhs):
      lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {}

    ~RelationalExpr() = default;

    ExprHolder lhs;
    Operator op;
    ExprHolder rhs;
};

struct AdditiveExpr: public Expr
{
  enum Operator {
    PLUS, MINUS,
  };

  AdditiveExpr(ExprHolder lhs, Operator op, ExprHolder rhs):
    lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} { std::cout << to_string() << std::endl; }

  ~AdditiveExpr() = default;

  std::string to_string() const override;

  ExprHolder lhs;
  Operator op;
  ExprHolder rhs;
};

struct MultiplicativeExpr: public Expr
{
  enum Operator {
    MUL, DIV,
  };

  MultiplicativeExpr() = default;

  MultiplicativeExpr(ExprHolder lhs, Operator op, ExprHolder rhs):
    lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {}

  ~MultiplicativeExpr() = default;

  std::string to_string() const override;

  ExprHolder lhs;
  Operator op;
  ExprHolder rhs;
};

struct ExponentialExpr: public Expr
{
  ExponentialExpr(ExprHolder lhs, ExprHolder rhs):
    lhs{std::move(lhs)}, rhs{std::move(rhs)} {}

  ~ExponentialExpr() = default;

  std::string to_string() const override;

  ExprHolder lhs;
  ExprHolder rhs;
};

}
