#include <CodeGen/CodeGen.hh>
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

namespace tlang::codegen {
using namespace _astnp_;
struct ExprVisitor: RecursiveASTVisitor<ExprVisitor, VisitorPattern::prePostOrder, VisitReturn<VisitStatus>> {
  ExprVisitor(CGContext &context) :
      context(context) {
  }

  visit_t visitUnaryOperation(UnaryOperation *node, bool isFirst) {
    using namespace llvm;
    auto kind = node->getOperator();
    if (isFirst) {
      if (kind == UnaryOperation::Minus)
        if (auto expr = dynamic_cast<LiteralExpr*>(node->getExpr()))
          expr->getValue() = "-" + expr->getValue();
      if (kind == UnaryOperation::Address)
        return visit_t::skip;
      return visit_value;
    }
    if (kind == UnaryOperation::Address) {
      if (auto dr = static_cast<DeclRefExpr*>(node->getExpr())) {
        context[node] = context[dr->getDecl().data()];
      } else
        context[node] = context[node->getExpr()];
    } else
      context[node] = context[node->getExpr()];
    return visit_value;
  }
  visit_t visitArrayExpr(ArrayExpr *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    auto type = context.emitType(node->getType());
    auto tmp = context.builder.CreateGEP(type, context[node->getArray()], context[node->getIndex(0)]);
    if (load)
      context[node] = context.builder.CreateLoad(type, tmp);
    else
      context[node] = tmp;
    return visit_value;
  }
  visit_t visitCallExpr(CallExpr *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    if (auto ref = dynamic_cast<DeclRefExpr*>(node->getCallee())) {
      Function *fun = context.module.getFunction(ref->getIdentifier());
      std::vector<Value*> ArgsV;
      for (unsigned i = 0, e = node->getArgs().size(); i != e; ++i) {
        auto arg = context[node->getArgs(i)];
        ArgsV.push_back(arg);
        if (!ArgsV.back())
          throw(std::runtime_error("Invalid call expr"));
      }
      context[node] = context.builder.CreateCall(fun, ArgsV);
    }
    return visit_value;
  }
  visit_t visitDeclRefExpr(DeclRefExpr *node, bool isFirst) {
    using namespace llvm;
    if (isFirst && node->getDecl()) {
      if (auto varDecl = dynamic_cast<VariableDecl*>(node->getDecl().data())) {
        if (varDecl->isRef) {
          auto *load = context.builder.CreateLoad(llvm::PointerType::get(context.emitType(varDecl->getType()), 0), context[varDecl],
              varDecl->getIdentifier().c_str());
          load = context.builder.CreateLoad(context.emitType(varDecl->getType()), load);
          context[node] = load;
        } else {
          auto *load = context.builder.CreateLoad(context.emitType(varDecl->getType()), context[varDecl], varDecl->getIdentifier().c_str());
          context[node] = load;
        }
      }
    }
    return visit_t::skip;
  }

};
}
