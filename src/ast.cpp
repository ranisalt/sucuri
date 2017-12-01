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
    //auto type = type_of(*this);

    auto type = llvm::Type::getPrimitiveType(
            context,
            llvm::Type::TypeID::PointerTyID
    );

    //return type;
    throw std::runtime_error("Name::to_llvm() not implemented. sry.");
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
    std::cout << "Assign.to_llvm()\n";
    if (not block) {
        throw std::runtime_error(
            "llvm::BasicBlock* not set for "s + name.to_string());
    }

    /*
    auto value = this->value.to_llvm();
    auto type = value->getType();
    {
        auto alloc_instr = builder.CreateAlloca(type, nullptr, name.to_string());
        {
            return builder.CreateStore(value, alloc_instr);
        }
    }
    */
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
    std::cout << "\n" << to_string() << ".to_llvm()\n\n";
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
    std::cout << "Function call -- to_llvm()\n";
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

llvm::Value* FunctionDefinition::to_llvm() const
{
    std::cout << "to_llvm::" << to_string() << "\n";

    std::cout << to_string() << "::\n    initializing param types..." << std::endl;
    auto param_types = std::vector<llvm::Type*>{};
    auto llvm_params = std::vector<llvm::Value*>{};
    llvm_params.reserve(param_types.size());

    for (auto& param: params) {
        auto type = llvm::Type::getPrimitiveType(
                context,
                llvm::Type::TypeID::PointerTyID
        );

        param_types.push_back(type);
    }

    std::cout << "    initializing llvm body... (which is: "
              << body->to_string() << ")" << std::endl;

    auto llvm_body = body->to_llvm();

    std::cout << "    initializing function type..." << std::endl;
    auto types = llvm::FunctionType::get(
        // return type
        llvm_body->getType(),
        // params types
        param_types,
        // is vararg?
        false
    );

    std::cout << "    initializing function itself..." << std::endl;
    auto function = llvm::Function::Create(
        types,
        llvm::GlobalVariable::LinkageTypes::ExternalLinkage,
        name.to_string(),
        block->getModule()
    );

    std::cout << "    done" << std::endl;
    return function;
}

std::string FunctionDefinition::to_string() const
{
    std::ostringstream os;
    os << "FunctionDefinition([";
    for (auto&& p: params) {
        os << p.to_string() << ", ";
    }
    os << "])";
    return os.str();
}


llvm::Value* Statement::to_llvm() const
{
    std::cout << "Statement::to_llvm() (for: " << state.to_string() << ")"
              << " (with block: " << block << ")\n";
    auto state = this->state;
    state.block = block;
    return state.to_llvm();
}

std::string Statement::to_string() const
{
    return "Statement::to_string()::"s + state.to_string();
}


llvm::Value* ReturnStatement::to_llvm() const
{
    std::cout << to_string() << "::to_llvm() (block: " << block << ")" << std::endl;;
    auto builder = llvm::IRBuilder<>{block};
    std::cout << "deu?" << std::endl;
    return builder.CreateRet(expr.to_llvm());
}

std::string ReturnStatement::to_string() const
{
    return "ReturnStatement(" + expr.to_string() + ")";
}


llvm::Value* StatementList::to_llvm() const
{
    std::cout << "StatementList::to_llvm()\n";
    auto block = llvm::BasicBlock::Create(context, to_string());
    for (auto statement: statements) {
        statement.block = block;
        statement.to_llvm();
    }
    return block;
}

std::string StatementList::to_string() const
{
    std::ostringstream os;
    for (auto&& state: statements) {
        os << "    " << state.to_string();
    }
    return "StatementList::to_string()::\n"s + os.str();
}


llvm::Value* Code::to_llvm() const
{
    std::cout << "Code::to_llvm()\n";
    return root.to_llvm();
}

std::string Code::to_string() const
{
    return root.to_string();
}


std::unique_ptr<llvm::Module> Program::to_llvm(llvm::IRBuilder<>& builder, llvm::LLVMContext& context) const
{
    std::cout << to_string() << ".to_llvm()\n";
    auto module = llvm::make_unique<llvm::Module>("test-module", AST::context);
    auto block = llvm::BasicBlock::Create(context, "top-level");
    root.to_llvm();
    return module;
}

std::string Program::to_string() const
{
    return "MainModule";
}

}
