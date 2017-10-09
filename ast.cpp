#include "ast.h"

namespace AST {

namespace {

static unsigned level = 0;

std::string make_indent() {
  return std::string(level, ' ');
}

}

std::ostream& operator<<(std::ostream& os, const Alias& rhs)
{
  return os << "alias='" << rhs.first << "'->'" << rhs.second << "'";
}

std::ostream& operator<<(std::ostream& os, const Import& rhs)
{
  auto indent = make_indent();
  return os << indent << "Import(from='" << rhs.path << "', " << rhs.name << ")";
}

std::ostream& operator<<(std::ostream& os, const ImportList& rhs)
{
  auto indent = make_indent();
  os << indent << "ImportList(\n";
  ++level;
  for (auto&& import: rhs) {
    os << import << ",\n";
  }
  --level;
  os << indent << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Statement& rhs)
{
  auto indent = make_indent();
  return os << indent << "Statement(\n" << indent << ")";
}

std::ostream& operator<<(std::ostream& os, const StatementList& rhs)
{
  auto indent = make_indent();
  os << indent << "StatementList(\n";
  ++level;
  for (auto&& stmt: rhs) {
    os << *stmt << ",\n";
  }
  --level;
  os << indent << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const Program& rhs)
{
  auto indent = make_indent();
  os << indent << "Program(\n";
  ++level;
  os << rhs.import_list << ",\n";
  os << rhs.stmt_list << ",\n";
  --level;
  os << indent << ")";
  return os;
}

}
