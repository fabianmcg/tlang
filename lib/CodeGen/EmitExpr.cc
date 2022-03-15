#include "CodeGen/CodeGen.hh"
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
  visit_t visitIntegerLiteral(IntegerLiteral *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    context[node] = ConstantInt::get(context.emitType(node->getType()), std::stoull(node->getValue()), true);
    return visit_value;
  }
  visit_t visitFloatLiteral(FloatLiteral *node, bool isFirst) {
    using namespace llvm;
    if (isFirst)
      return visit_value;
    context[node] = ConstantFP::get(context.emitType(node->getType()), std::stod(node->getValue()));
    return visit_value;
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
//      std::cerr << node->getSourceRange().to_string() << " " << ref->getSourceRange().to_string() << std::endl;
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
        auto *load = context.builder.CreateLoad(context.emitType(varDecl->getType()), context[varDecl], varDecl->getIdentifier().c_str());
        context[node] = load;
      }
    }
    return visit_t::skip;
  }
  visit_t visitParenExpr(ParenExpr *node, bool isFirst) {
    if (!isFirst)
      context[node] = context[node->getExpr()];
    return visit_value;
  }
  visit_t visitBinaryOperation(BinaryOperation *node, bool isFirst) {
    using namespace llvm;
    if (isFirst) {
      auto LHS = node->getLhs();
      load = true;
      dynamicTraverse(node->getRhs());
      if (node->getOperator() != OperatorKind::Assign || LHS->isNot(NodeClass::DeclRefExpr)) {
        if (node->getOperator() == OperatorKind::Assign)
          load = false;
        dynamicTraverse(LHS);
      }
      load = true;
    } else {
      switch (node->getOperator()) {
      case OperatorKind::Plus: {
        auto &LHS = context[node->getLhs()];
        auto &RHS = context[node->getRhs()];
        if (LHS->getType()->isFloatingPointTy())
          context[node] = context.builder.CreateFAdd(LHS, RHS);
        else
          context[node] = context.builder.CreateAdd(LHS, RHS);
        break;
      }
      case OperatorKind::Minus: {
        auto &LHS = context[node->getLhs()];
        auto &RHS = context[node->getRhs()];
        if (LHS->getType()->isFloatingPointTy())
          context[node] = context.builder.CreateFSub(LHS, RHS);
        else
          context[node] = context.builder.CreateSub(LHS, RHS);
        break;
      }
      case OperatorKind::Multiply: {
        auto &LHS = context[node->getLhs()];
        auto &RHS = context[node->getRhs()];
        if (LHS->getType()->isFloatingPointTy())
          context[node] = context.builder.CreateFMul(LHS, RHS);
        else
          context[node] = context.builder.CreateMul(LHS, RHS);
        break;
      }
      case OperatorKind::Divide: {
        auto &LHS = context[node->getLhs()];
        auto &RHS = context[node->getRhs()];
        if (LHS->getType()->isFloatingPointTy())
          context[node] = context.builder.CreateFDiv(LHS, RHS);
        else
          context[node] = context.builder.CreateSDiv(LHS, RHS);
        break;
      }
      case OperatorKind::Equal: {
        auto &LHS = context[node->getLhs()];
        auto &RHS = context[node->getRhs()];
        if (LHS->getType()->isFloatingPointTy())
          context[node] = context.builder.CreateFCmpOEQ(LHS, RHS);
        else
          context[node] = context.builder.CreateICmpEQ(LHS, RHS);
        break;
      }
      case OperatorKind::NEQ: {
        auto &LHS = context[node->getLhs()];
        auto &RHS = context[node->getRhs()];
        if (LHS->getType()->isFloatingPointTy())
          context[node] = context.builder.CreateFCmpONE(LHS, RHS);
        else
          context[node] = context.builder.CreateICmpNE(LHS, RHS);
        break;
      }
      case OperatorKind::Less: {
        auto &LHS = context[node->getLhs()];
        auto &RHS = context[node->getRhs()];
        if (LHS->getType()->isFloatingPointTy())
          context[node] = context.builder.CreateFCmpOLT(LHS, RHS);
        else
          context[node] = context.builder.CreateICmpSLT(LHS, RHS);
        break;
      }
      case OperatorKind::LEQ: {
        auto &LHS = context[node->getLhs()];
        auto &RHS = context[node->getRhs()];
        if (LHS->getType()->isFloatingPointTy())
          context[node] = context.builder.CreateFCmpOLE(LHS, RHS);
        else
          context[node] = context.builder.CreateICmpSLE(LHS, RHS);
        break;
      }
      case OperatorKind::Assign: {
        auto &LHS = context[node->getLhs()];
        auto &RHS = context[node->getRhs()];
        if (LHS == nullptr) {
          if (auto ref = dynamic_cast<DeclRefExpr*>(node->getLhs()))
            if (auto varDecl = dynamic_cast<VariableDecl*>(ref->getDecl().data())) {
              LHS = context.builder.CreateStore(RHS, context[varDecl]);
              context[node] = LHS;
            }
        } else
          context[node] = context.builder.CreateStore(RHS, LHS);
        break;
      }
      default:
        break;
      }
    }
    return visit_t::skip;
  }
  llvm::Value* emit(Expr *node) {
    dynamicTraverse(node);
    return context[node];
  }
  CGContext &context;
  bool load = true;
};
llvm::Value* CGContext::emitExpr(Expr *expr) {
  return ExprVisitor { *this }.emit(expr);
}
}
