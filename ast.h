#pragma once

#include <ostream>
#include <memory>
#include <vector>
#include <string>

namespace AST {

using Alias = std::pair<std::string, std::string>;

std::ostream& operator<<(std::ostream& os, const Alias& rhs);

class Import
{
  public:
    Import() = default;

    Import(Alias name):
      name{std::move(name)} {}

    Import(std::string path, Alias name):
      path{std::move(path)}, name{std::move(name)} {}

    std::string to_string() const;

    std::string path;
    Alias name;
};

using ImportList = std::vector<Import>;

class Expr
{
  public:
    Expr() = default;

    Expr(const Expr& stmt):
      self{stmt.self->copy()} {}

    template<class T>
    Expr(T expr):
      self{new model<T>{std::move(expr)}} {}

    Expr& operator=(const Expr& stmt)
    { self.reset(stmt.self->copy()); return *this; }

    std::string to_string() const
    {
      return self->to_string();
    }

  private:
    struct concept {
      virtual ~concept() = default;
      virtual concept* copy() const = 0;
      virtual std::string to_string() const = 0;
    };

    template<class T>
    struct model: concept {
      model(T data): data{std::move(data)} {}
      virtual model<T>* copy() const
      { return new model<T>{data}; }
      virtual std::string to_string() const
      { return data.to_string(); }

      T data;
    };

    std::unique_ptr<concept> self;
};

class AssignmentExpr
{
  public:
    AssignmentExpr() = default;

    AssignmentExpr(std::string name, Expr value):
      name{std::move(name)}, value{std::move(value)} {}

    std::unique_ptr<AssignmentExpr> copy() const;

    std::string to_string() const;

    std::string name;
    Expr value;
};

class Statement
{
  public:
    Statement() = default;

    Statement(const Statement& stmt):
      self{stmt.self->copy()} {}

    template<class T>
    Statement(T expr):
      self{new model<T>{std::move(expr)}} {}

    Statement& operator=(const Statement& stmt)
    { self.reset(stmt.self->copy()); return *this; }

    std::string to_string() const
    {
      return self->to_string();
    }

  private:
    struct concept {
      virtual ~concept() = default;
      virtual concept* copy() const = 0;
      virtual std::string to_string() const = 0;
    };

    template<class T>
    struct model: concept {
      model(T data): data{std::move(data)} {}
      virtual model<T>* copy() const
      { return new model<T>{data}; }
      virtual std::string to_string() const
      { return data.to_string(); }

      T data;
    };

    std::unique_ptr<concept> self;
};

using StatementList = std::vector<Statement>;

class Program
{
  public:
    Program() = default;

    Program(StatementList stmt_list):
      import_list{}, stmt_list{std::move(stmt_list)} {}

    Program(ImportList import_list, StatementList stmt_list):
      import_list{std::move(import_list)}, stmt_list{std::move(stmt_list)} {}

    std::string to_string() const;

    ImportList import_list;
    StatementList stmt_list;
};

class Float
{
  public:
    Float(long double value): value{value} {}

    std::string to_string() const;

    const long double value;
};

class Integer
{
  public:
    Integer(long long value): value{value} {}

    std::string to_string() const;

    const long long value;
};

class String
{
  public:
    String(std::string value): value{std::move(value)} {}

    std::string to_string() const;

    const std::string value;
};

class Literal
{
  public:
    Literal() = default;

    Literal(const Literal& stmt):
      self{stmt.self->copy()} {}

    template<class T>
    Literal(T expr):
      self{new model<T>{std::move(expr)}} {}

    Literal& operator=(const Literal& stmt)
    { self.reset(stmt.self->copy()); return *this; }

    std::string to_string() const
    {
      return self->to_string();
    }

  private:
    struct concept {
      virtual ~concept() = default;
      virtual concept* copy() const = 0;
      virtual std::string to_string() const = 0;
    };

    template<class T>
    struct model: concept {
      model(T data): data{std::move(data)} {}
      virtual model<T>* copy() const
      { return new model<T>{data}; }
      virtual std::string to_string() const
      { return data.to_string(); }

      T data;
    };

    std::unique_ptr<concept> self;
};

class UnaryExpr
{
  public:
    enum Operator {
      NOT, MINUS,
    };

    UnaryExpr() = default;

    UnaryExpr(Operator op, Expr rhs):
      op{op}, rhs{std::move(rhs)} {}

    std::string to_string() const;

    Operator op;
    Expr rhs;
};

/* class LogicalExpr: public Expr */
/* { */
/*   public: */
/*     enum Operator { */
/*       AND, OR, XOR, */
/*     }; */

/*     LogicalExpr(std::unique_ptr<Expr> lhs, Operator op, std::unique_ptr<Expr> rhs): */
/*       lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {} */

/*     std::unique_ptr<Expr> lhs; */
/*     Operator op; */
/*     std::unique_ptr<Expr> rhs; */
/* }; */

/* class EqualityExpr: public Expr */
/* { */
/*   public: */
/*     enum Operator { */
/*       EQ, NE, */
/*     }; */

/*     EqualityExpr(std::unique_ptr<Expr> lhs, Operator op, std::unique_ptr<Expr> rhs): */
/*       lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {} */

/*     std::unique_ptr<Expr> lhs; */
/*     Operator op; */
/*     std::unique_ptr<Expr> rhs; */
/* }; */

/* class RelationalExpr: public Expr */
/* { */
/*   public: */
/*     enum Operator { */
/*       LT, LE, GT, GE, */
/*     }; */

/*     RelationalExpr(std::unique_ptr<Expr> lhs, Operator op, std::unique_ptr<Expr> rhs): */
/*       lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {} */

/*     std::unique_ptr<Expr> lhs; */
/*     Operator op; */
/*     std::unique_ptr<Expr> rhs; */
/* }; */

/* class AdditiveExpr: public Expr */
/* { */
/*   public: */
/*     enum Operator { */
/*       PLUS, MINUS, */
/*     }; */

/*     AdditiveExpr(std::unique_ptr<Expr> lhs, Operator op, std::unique_ptr<Expr> rhs): */
/*       lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {} */

/*     std::unique_ptr<Expr> lhs; */
/*     Operator op; */
/*     std::unique_ptr<Expr> rhs; */
/* }; */

class MultiplicativeExpr
{
  public:
    enum Operator {
      MUL, DIV,
    };

    MultiplicativeExpr() = default;

    MultiplicativeExpr(Expr lhs, Operator op, Expr rhs):
      lhs{std::move(lhs)}, op{op}, rhs{std::move(rhs)} {}

    std::string to_string() const;

    Expr lhs;
    Operator op;
    Expr rhs;
};

/* class ExponentialExpr: public Expr */
/* { */
/* }; */

}
