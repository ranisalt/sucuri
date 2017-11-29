#include "ast.h"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/IR/Constants.h"

#include <sstream>

using namespace std::literals;

auto type_of(const AST::Node& node) {
    return llvm::Type::TypeID::StructTyID;
}

namespace AST {

llvm::Value* Name::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
    using namespace llvm;
    auto type = type_of(*this);
    return nullptr;
}

std::string Name::to_string() const
{
  std::ostringstream os;
  auto it = path.begin();
  os << *it;
  while (++it != path.end()) {
    os << "." << *it;
  }
  return os.str();
}

llvm::Value* AssignmentExpr::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
    // TODO
    throw std::runtime_error("Not implemented.");
}

std::string AssignmentExpr::to_string() const
{
  std::ostringstream os;
  os << "AssignmentExpr(" << name.to_string() << '=' << value.to_string() << ")";
  return os.str();
}

llvm::Value* Float::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
  return llvm::ConstantFP::get(context, llvm::APFloat(value));
}

std::string Float::to_string() const
{
  std::ostringstream os;
  os << "Float(" << value << ")";
  return os.str();
}

llvm::Value* Integer::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
  return llvm::ConstantInt::get(context, llvm::APSInt(value));
}

std::string Integer::to_string() const
{
  std::ostringstream os;
  os << "Integer(" << value << ")";
  return os.str();
}


llvm::Value* Bool::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
    return llvm::ConstantInt::get(context, llvm::APSInt(value));
}

std::string Bool::to_string() const
{
  std::ostringstream os;
  os << "Bool(" << std::boolalpha << value << ")";
  return os.str();
}


llvm::Value* String::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
    return llvm::ConstantDataArray::getString(context, llvm::StringRef(value));
}

std::string String::to_string() const
{
  std::ostringstream os;
  os << "String(" << value << ")";
  return os.str();
}

llvm::Value* ExponentialExpr::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
  throw std::runtime_error(__PRETTY_FUNCTION__ + " not implemented."s);

/*   llvm::Value* L = rhs.to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context); */
/*   if (not L) { */
/*     return nullptr; */
/*   } */

/*   llvm::Value* R = rhs.to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context); */
/*   if (not R) { */
/*     return nullptr; */
/*   } */

/*   switch (op) { */
/*     case NOT: return builder.CreateNot(R); */
/*     case NEG: return builder.CreateNeg(R); */
/*   } */

/*   throw std::runtime_error("Invalid operator"); */
}

std::string ExponentialExpr::to_string() const
{
  std::ostringstream os;
  os << "ExponentialExpr(" << lhs.to_string() << " ** " << rhs.to_string() << ")";
  return os.str();
}

llvm::Value* UnaryExpr::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
  llvm::Value* R = rhs.to_llvm(builder, context);
  if (not R) {
    return nullptr;
  }

  switch (op) {
    case NOT: return builder.CreateNot(R);
    case NEG: return builder.CreateNeg(R);
  }

  throw std::runtime_error("Invalid operator");
}

std::string UnaryExpr::to_string() const
{
  std::ostringstream os;
  os << "UnaryExpr(";
  switch (op) {
    case NOT: os << "not "; break;
    case NEG: os << "- "; break;
  }
  os << rhs.to_string() << ")";
  return os.str();
}

llvm::Value* LogicalExpr::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
  llvm::Value* L = lhs.to_llvm(builder, context);
  if (not L) {
    return nullptr;
  }

  llvm::Value* R = rhs.to_llvm(builder, context);
  if (not R) {
    return nullptr;
  }

  switch (op) {
    case AND: return builder.CreateAnd(L, R);
    case OR: return builder.CreateOr(L, R);
    case XOR: return builder.CreateXor(L, R);
  }

  throw std::runtime_error("Invalid operator");
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

llvm::Value* EqualityExpr::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
  {
    const Integer* L = lhs.as<Integer>();
    const Integer* R = rhs.as<Integer>();

    if (L and R) {
      switch (op) {
        case EQ: return builder.CreateICmpEQ(L->to_llvm(builder, context), R->to_llvm(builder, context));
        case NE: return builder.CreateICmpNE(L->to_llvm(builder, context), R->to_llvm(builder, context));
      }
    }
  }

  {
    const Float* L = lhs.as<Float>();
    const Float* R = rhs.as<Float>();

    if (L and R) {
      switch (op) {
        case EQ: return builder.CreateFCmpUEQ(L->to_llvm(builder, context), R->to_llvm(builder, context));
        case NE: return builder.CreateFCmpUNE(L->to_llvm(builder, context), R->to_llvm(builder, context));
      }
    }
  }

  throw std::runtime_error("Invalid operator");
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

llvm::Value* RelationalExpr::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
  {
    const Integer* L = lhs.as<Integer>();
    const Integer* R = rhs.as<Integer>();

    if (L and R) {
      switch (op) {
        case LT: return builder.CreateICmpSLT(L->to_llvm(builder, context), R->to_llvm(builder, context));
        case LE: return builder.CreateICmpSLE(L->to_llvm(builder, context), R->to_llvm(builder, context));
        case GT: return builder.CreateICmpSGT(L->to_llvm(builder, context), R->to_llvm(builder, context));
        case GE: return builder.CreateICmpSGE(L->to_llvm(builder, context), R->to_llvm(builder, context));
      }
    }
  }

  {
    const Float* L = lhs.as<Float>();
    const Float* R = rhs.as<Float>();

    if (L and R) {
      switch (op) {
        case LT: return builder.CreateFCmpULT(L->to_llvm(builder, context), R->to_llvm(builder, context));
        case LE: return builder.CreateFCmpULE(L->to_llvm(builder, context), R->to_llvm(builder, context));
        case GT: return builder.CreateFCmpUGT(L->to_llvm(builder, context), R->to_llvm(builder, context));
        case GE: return builder.CreateFCmpUGE(L->to_llvm(builder, context), R->to_llvm(builder, context));
      }
    }
  }

  throw std::runtime_error("Invalid operator");
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

llvm::Value* AdditiveExpr::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
  llvm::Value* L = lhs.to_llvm(builder, context);
  if (not L) {
    return nullptr;
  }

  llvm::Value* R = rhs.to_llvm(builder, context);
  if (not R) {
    return nullptr;
  }

  switch (op) {
    case PLUS: return builder.CreateAdd(L, R);
    case MINUS: return builder.CreateSub(L, R);
  }

  throw std::runtime_error("Invalid operator");
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

llvm::Value* MultiplicativeExpr::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
  llvm::Value* L = lhs.to_llvm(builder, context);
  if (not L) {
    return nullptr;
  }

  llvm::Value* R = rhs.to_llvm(builder, context);
  if (not R) {
    return nullptr;
  }

  switch (op) {
    case MUL: return builder.CreateMul(L, R);
    case DIV: return builder.CreateSDiv(L, R);
  }

  throw std::runtime_error("Invalid operator");
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

llvm::Value* VariableDecl::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
    // TODO
    throw std::runtime_error("Not implemented.");
}

std::string VariableDecl::to_string() const
{
  std::ostringstream os;
  os << "VariableDecl(" << expr.to_string() << ")";
  return os.str();
}


llvm::Value* ListExpr::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
    // TODO
    throw std::runtime_error("Not implemented.");
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


llvm::Value* FunctionCall::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
    // TODO
    throw std::runtime_error("Not implemented.");
}

std::string FunctionCall::to_string() const
{
  std::ostringstream os;
  os << "FunctionCall(name=" << name.to_string() << ", args=[";
  for (auto&& e: expr_list) {
    os << e.to_string() << ", ";
  }
  os << "])";
  return os.str();
}


std::unique_ptr<llvm::Module> Program::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
    auto module = llvm::make_unique<llvm::Module>("test-module", context);
    return module;
}

}
