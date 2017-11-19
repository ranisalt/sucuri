#include "ast.h"

#include <sstream>

namespace AST {

std::string Identifier::to_string() const
{
  std::ostringstream os;
  os << "Identifier(" << value << ")";
  return os.str();
}

std::string Name::to_string() const
{
  std::ostringstream os;
  auto it = path.begin();
  os << it->to_string();
  while (++it != path.end()) {
    os << "." << it->to_string();
  }
  return os.str();
}

std::string AssignmentExpr::to_string() const
{
  std::ostringstream os;
  os << "AssignmentExpr(" << name.to_string() << '=' << value.to_string() << ")";
  return os.str();
}

std::string Float::to_string() const
{
  std::ostringstream os;
  os << "Float(" << value << ")";
  return os.str();
}

std::string Integer::to_string() const
{
  std::ostringstream os;
  os << "Integer(" << value << ")";
  return os.str();
}

std::string Bool::to_string() const
{
  std::ostringstream os;
  os << "Bool(" << value << ")";
  return os.str();
}

std::string String::to_string() const
{
  std::ostringstream os;
  os << "String(" << value << ")";
  return os.str();
}

std::string ExponentialExpr::to_string() const
{
  std::ostringstream os;
  os << "ExponentialExpr(" << lhs.to_string() << " ** " << rhs.to_string() << ")";
  return os.str();
}

std::string UnaryExpr::to_string() const
{
  std::ostringstream os;
  os << "UnaryExpr(";
  switch (op) {
    case NOT: os << "not "; break;
    case MINUS: os << "- "; break;
  }
  os << rhs.to_string() << ")";
  return os.str();
}

std::string LogicalExpr::to_string() const
{
  std::ostringstream os;
  os << "LogicalExpr(" << lhs.to_string();
  switch (op) {
    case AND: os << " and "; break;
    case OR: os << " or "; break;
    case XOR: os << " xor "; break;
  }
  os << rhs.to_string() << ")";
  return os.str();
}

std::string EqualityExpr::to_string() const
{
  std::ostringstream os;
  os << "EqualityExpr(" << lhs.to_string();
  switch (op) {
    case EQ: os << " = "; break;
    case NE: os << " != "; break;
  }
  os << rhs.to_string() << ")";
  return os.str();
}

std::string RelationalExpr::to_string() const
{
  std::ostringstream os;
  os << "RelationalExpr(" << lhs.to_string();
  switch (op) {
    case LT: os << " < "; break;
    case LE: os << " <= "; break;
    case GT: os << " > "; break;
    case GE: os << " >= "; break;
  }
  os << rhs.to_string() << ")";
  return os.str();
}

std::string AdditiveExpr::to_string() const
{
  std::ostringstream os;
  os << "AdditiveExpr(" << lhs.to_string();
  switch (op) {
    case PLUS: os << " + "; break;
    case MINUS: os << " - "; break;
  }
  os << rhs.to_string() << ")";
  return os.str();
}

std::string MultiplicativeExpr::to_string() const
{
  std::ostringstream os;
  os << "MultiplicativeExpr(" << lhs.to_string();
  switch (op) {
    case MUL: os << " * "; break;
    case DIV: os << " / "; break;
  }
  os << rhs.to_string() << ")";
  return os.str();
}

std::string VariableDecl::to_string() const
{
  std::ostringstream os;
  os << "VariableDecl(" << expr.to_string() << ")";
  return os.str();
}

std::string ListExpr::to_string() const
{
  std::ostringstream os;
  os << "ListExpr([";
  for (auto&& v: values) {
    os << v.to_string() << ", ";
  }
  os << "])";
  return os.str();
}

std::string FunctionCall::to_string() const
{
  std::ostringstream os;
  os << "FunctionCall(";
  for (auto&& e: expr_list) {
    os << e.to_string() << ", ";
  }
  os << ")";
  return os.str();
}

}
