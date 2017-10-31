#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <string>

namespace AST {

class Node
{
  public:
    constexpr Node() = default;

    Node(const Node& other) = default;

    template<class T>
    Node(T data):
      ptr{std::make_shared<model<T>>(std::move(data))} {}

    Node& operator=(const Node& rhs) = default;

    template<class T>
    Node& operator=(T data)
    { ptr = std::make_shared<model<T>>(std::move(data)); return *this; }

    virtual std::string to_string() const {
        return ptr ? ptr->to_string() : "ptr null";
    }

  private:
  public:
    struct concept {
      virtual ~concept() = default;
      virtual std::unique_ptr<concept> copy() const = 0;
      virtual std::string to_string() const = 0;
    };

    template<class T>
      struct model final: concept {
        model(T data): data{std::move(data)} {}
        std::unique_ptr<concept> copy() const {
            return std::make_unique<model<T>>(data);
        }
        std::string to_string() const {
            return data.to_string();
        }

        T data;
      };

    std::shared_ptr<concept> ptr;
};

struct Identifier
{
  Identifier() = default;

  Identifier(std::string value): value{std::move(value)} {
      std::cout << to_string() << std::endl;
  }

  std::string to_string() const;

  std::string value;
};

struct Name
{
  Name() = default;

  Name(std::vector<Identifier> path): path{std::move(path)} {}

  std::string to_string() const;

  void append(Identifier i) {
      path.push_back(std::move(i));
  }

  std::vector<Identifier> path;
};

struct Alias {
    Alias() = default;

    Alias(Name name):
        alias{name, name.path[0]}
    {}

    Alias(std::pair<Name, Identifier> alias):
        alias{alias}
    {}

    std::string to_string() const;

    std::pair<Name, Identifier> alias;
};

inline bool operator==(const Name lhs, const Name rhs) {
    return lhs.to_string() == rhs.to_string();
}

struct Float
{
  Float(long double value): value{value} {
      std::cout << to_string() << std::endl;
  }

  std::string to_string() const;

  long double value;
};

struct Integer
{
  Integer(long long value): value{value} {
      std::cout << to_string() << std::endl;
  }

  std::string to_string() const;

  long long value;
};

struct Bool
{
    Bool(bool value): value{value} {
        std::cout << to_string() << std::endl;
    }

    std::string to_string() const;

    bool value;
};

struct String
{
  String(std::string value): value{std::move(value)} {
      std::cout << to_string() << std::endl;
  }

  std::string to_string() const;

  std::string value;
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

struct AssignmentExpr;
struct UnaryExpr;
struct LogicalExpr;
struct EqualityExpr;
struct RelationalExpr;
struct AdditiveExpr;
struct MultiplicativeExpr;
struct ExponentialExpr;

/* struct Expr: public Node { */
/*   Expr() = default; */

/*   Expr& operator=(Name value); */
/*   Expr& operator=(AssignmentExpr value); */
/*   Expr& operator=(UnaryExpr value); */
/*   Expr& operator=(LogicalExpr value); */
/*   Expr& operator=(EqualityExpr value); */
/*   Expr& operator=(RelationalExpr value); */
/*   Expr& operator=(AdditiveExpr value); */
/*   Expr& operator=(MultiplicativeExpr value); */
/*   Expr& operator=(ExponentialExpr value); */
/* }; */

struct AssignmentExpr
{
  AssignmentExpr() = default;
  AssignmentExpr(Name name, Node value):
    name{std::move(name)}, value{std::move(value)} {
        std::cout << to_string() << std::endl;
    }

  std::string to_string() const;

  Name name;
  Node value;
};

struct UnaryExpr
{
  enum Operator {
    NOT, MINUS,
  };

  UnaryExpr() = default;

  UnaryExpr(Operator op, Node rhs):
    op{op}, rhs{std::move(rhs)} {
        std::cout << to_string() << std::endl;
    }

  std::string to_string() const;

  Operator op;
  Node rhs;
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

  std::string to_string() const;

  Node lhs;
  Operator op;
  Node rhs;
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

  std::string to_string() const;

  Node lhs;
  Operator op;
  Node rhs;
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

  std::string to_string() const;

  Node lhs;
  Operator op;
  Node rhs;
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

  std::string to_string() const;

  Node lhs;
  Operator op;
  Node rhs;
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

  std::string to_string() const;

  Node lhs;
  Operator op;
  Node rhs;
};

struct ExponentialExpr
{
  ExponentialExpr(Node lhs, Node rhs):
    lhs{std::move(lhs)}, rhs{std::move(rhs)} {
        std::cout << to_string() << std::endl;
    }

  std::string to_string() const;

  Node lhs;
  Node rhs;
};

struct Decl: public Node {};

struct VariableDecl: public Decl
{
  explicit VariableDecl(Node expr): expr{std::move(expr)} {
      std::cout << to_string() << std::endl;
  }

  std::string to_string() const;

  Node expr;
};

}
