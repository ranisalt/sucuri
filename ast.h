#pragma once

#include <ostream>
#include <memory>
#include <vector>
#include <string>

namespace AST {

using Alias = std::pair<std::string, std::string>;

std::ostream& operator<<(std::ostream& os, const Alias& rhs);

class Node: public std::enable_shared_from_this<Node>
{
};

class Import: public Node
{
  public:
    Import() = default;

    Import(Alias name):
      name{std::move(name)} {}

    Import(std::string path, Alias name):
      path{std::move(path)}, name{std::move(name)} {}

    std::string path;
    Alias name;
};

std::ostream& operator<<(std::ostream& os, const Import& rhs);

using ImportList = std::vector<Import>;

std::ostream& operator<<(std::ostream& os, const ImportList& rhs);

class Statement: public Node
{
};

std::ostream& operator<<(std::ostream& os, const Statement& rhs);

using StatementList = std::vector<std::shared_ptr<Statement>>;

std::ostream& operator<<(std::ostream& os, const StatementList& rhs);

class Program: public Node
{
  public:
    Program() = default;

    Program(StatementList stmt_list):
      import_list{}, stmt_list{std::move(stmt_list)} {}

    Program(ImportList import_list, StatementList stmt_list):
      import_list{std::move(import_list)}, stmt_list{std::move(stmt_list)} {}

    ImportList import_list;
    StatementList stmt_list;
};

std::ostream& operator<<(std::ostream& os, const Program& rhs);

class Expr: public Node
{
};

class Literal: public Expr {};

class Float: public Literal
{
  public:
    Float(long double value): value{value} {}

    const long double value;
};

class Integer: public Literal
{
  public:
    Integer(long long value): value{value} {}

    const long long value;
};

class String: public Literal
{
  public:
    String(std::string value): value{std::move(value)} {}

    const std::string value;
};

class UnaryExpr: public Expr
{
  public:
    enum Operator {
      NOT, MINUS,
    };

    UnaryExpr() = default;

    UnaryExpr(Operator op, std::shared_ptr<Node>&& rhs):
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

    MultiplicativeExpr() = default;

    MultiplicativeExpr(std::shared_ptr<Node>&& lhs, Operator op, std::shared_ptr<Node>&& rhs):
      lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {}

    std::shared_ptr<Node> lhs;
    Operator op;
    std::shared_ptr<Node> rhs;
};

/* class ExponentialExpr: public Expr */
/* { */
/* }; */

}
