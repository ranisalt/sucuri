#pragma once

#include <iostream>
#include <memory>
#include <string>

namespace AST {

class Node: public std::enable_shared_from_this<Node>
{
};

class Expr: public Node
{
};

class Literal: public Expr {};

class Float: public Literal
{
  public:
    Float(long double value): value{value} {
      std::cout << "Float: " << value << std::endl;
    }

    const long double value;
};

class Integer: public Literal
{
  public:
    Integer(long long value): value{value} {
      std::cout << "Integer: " << value << std::endl;
    }

    const long long value;
};

class String: public Literal
{
  public:
    String(std::string value): value{std::move(value)} {
      std::cout << "String: " << this->value << std::endl;
    }

    const std::string value;
};

class UnaryExpr: public Expr
{
  public:
    enum Operator {
      NOT, MINUS,
    };

    UnaryExpr(Operator op, std::shared_ptr<Node> rhs):
      op{op}, rhs{std::move(rhs)} {}

    Operator op;
    std::shared_ptr<Node> rhs;
};

/* class LogicalExpr: public Expr */
/* { */
/*   public: */
/*     enum Operator { */
/*       AND, OR, XOR, */
/*     }; */

/*     LogicalExpr(std::shared_ptr<Expr> lhs, Operator op, std::shared_ptr<Expr> rhs): */
/*       lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {} */

/*     std::shared_ptr<Expr> lhs; */
/*     Operator op; */
/*     std::shared_ptr<Expr> rhs; */
/* }; */

/* class EqualityExpr: public Expr */
/* { */
/*   public: */
/*     enum Operator { */
/*       EQ, NE, */
/*     }; */

/*     EqualityExpr(std::shared_ptr<Expr> lhs, Operator op, std::shared_ptr<Expr> rhs): */
/*       lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {} */

/*     std::shared_ptr<Expr> lhs; */
/*     Operator op; */
/*     std::shared_ptr<Expr> rhs; */
/* }; */

/* class RelationalExpr: public Expr */
/* { */
/*   public: */
/*     enum Operator { */
/*       LT, LE, GT, GE, */
/*     }; */

/*     RelationalExpr(std::shared_ptr<Expr> lhs, Operator op, std::shared_ptr<Expr> rhs): */
/*       lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {} */

/*     std::shared_ptr<Expr> lhs; */
/*     Operator op; */
/*     std::shared_ptr<Expr> rhs; */
/* }; */

/* class AdditiveExpr: public Expr */
/* { */
/*   public: */
/*     enum Operator { */
/*       PLUS, MINUS, */
/*     }; */

/*     AdditiveExpr(std::shared_ptr<Expr> lhs, Operator op, std::shared_ptr<Expr> rhs): */
/*       lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {} */

/*     std::shared_ptr<Expr> lhs; */
/*     Operator op; */
/*     std::shared_ptr<Expr> rhs; */
/* }; */

class MultiplicativeExpr: public Expr
{
  public:
    enum Operator {
      MUL, DIV,
    };

    MultiplicativeExpr(std::shared_ptr<Node> lhs, Operator op, std::shared_ptr<Node> rhs):
      lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {}

    std::shared_ptr<Node> lhs;
    Operator op;
    std::shared_ptr<Node> rhs;
};

/* class ExponentialExpr: public Expr */
/* { */
/* }; */

}
