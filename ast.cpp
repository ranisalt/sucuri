#include "ast.h"

#include <sstream>

namespace AST {

namespace {

/* static unsigned level = 0; */

/* std::string make_indent() { */
/*   return std::string(level, ' '); */
/* } */

/* } */

/* std::ostream& operator<<(std::ostream& os, const Alias& rhs) */
/* { */
/*   return os << "alias='" << rhs.first << "'->'" << rhs.second << "'"; */
/* } */

/* std::ostream& operator<<(std::ostream& os, const ImportList& rhs) */
/* { */
/*   auto indent = make_indent(); */
/*   os << indent << "ImportList(\n"; */
/*   ++level; */
/*   for (auto&& import: rhs) { */
/*     os << import.to_string() << ",\n"; */
/*   } */
/*   --level; */
/*   os << indent << ")"; */
/*   return os; */
/* } */

/* std::ostream& operator<<(std::ostream& os, const StatementList& rhs) */
/* { */
/*   auto indent = make_indent(); */
/*   os << indent << "StatementList(\n"; */
/*   ++level; */
/*   for (auto&& stmt: rhs) { */
/*     os << stmt.to_string() << ",\n"; */
/*   } */
/*   --level; */
/*   os << indent << ")"; */
/*   return os; */
/* } */

/* std::string Program::to_string() const */
/* { */
/*   std::ostringstream os; */
/*   auto indent = make_indent(); */
/*   os << indent << "Program(\n"; */
/*   ++level; */
/*   os << import_list << ",\n" << stmt_list << ",\n"; */
/*   --level; */
/*   os << indent << ")"; */
/*   return os.str(); */
/* } */

/* std::string AssignmentExpr::to_string() const */
/* { */
/*   auto indent = make_indent(); */
/*   std::ostringstream os; */
/*   os << indent << "AssignmentExpr(" << name << '=' << value->to_string() << ")"; */
/*   return os.str(); */
/* } */

/* std::string Float::to_string() const */
/* { */
/*   auto indent = make_indent(); */
/*   std::ostringstream os; */
/*   os << indent << "Float(" << value << ")"; */
/*   return os.str(); */
/* } */

/* std::string UnaryExpr::to_string() const */
/* { */
/*   auto indent = make_indent(); */
/*   std::ostringstream os; */
/*   os << indent << "UnaryExpr("; */
/*   switch (op) { */
/*     case NOT: os << "not "; break; */
/*     case MINUS: os << "- "; break; */
/*   } */
/*   os << rhs->to_string() << ")"; */
/*   return os.str(); */
/* } */

/* std::string MultiplicativeExpr::to_string() const */
/* { */
/*   auto indent = make_indent(); */
/*   std::ostringstream os; */
/*   os << indent << "MultiplicativeExpr(" << lhs->to_string(); */
/*   switch (op) { */
/*     case MUL: os << " * "; break; */
/*     case DIV: os << " / "; break; */
/*   } */
/*   os << rhs->to_string() << ")"; */
/*   return os.str(); */
/* } */

}
