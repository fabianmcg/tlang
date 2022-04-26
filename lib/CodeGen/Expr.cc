#include <AST/Decl.hh>
#include <AST/Expr.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>
#include <AST/Api.hh>
#include <CodeGen/GenericEmitter.hh>
#include <Support/Enumerate.hh>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/Optional.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

namespace tlang::codegen {
llvm::Value* GenericEmitter::makeInt32(int64_t value, bool signedQ) {
  return llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), value, signedQ);
}
IRType_t<BooleanLiteral> GenericEmitter::makeBooleanLiteral(BooleanLiteral *literal) {
  uint64_t value = static_cast<uint64_t>(literal->getValue());
  return llvm::ConstantInt::get(emitQualType(literal->getType()), value, false);
}
IRType_t<IntegerLiteral> GenericEmitter::makeIntegerLiteral(IntegerLiteral *literal) {
  uint64_t value = static_cast<uint64_t>(literal->getValue());
  return llvm::ConstantInt::get(emitQualType(literal->getType()), value, true);
}
IRType_t<UIntegerLiteral> GenericEmitter::makeIntegerLiteral(UIntegerLiteral *literal) {
  uint64_t value = static_cast<uint64_t>(literal->getValue());
  return llvm::ConstantInt::get(emitQualType(literal->getType()), value, false);
}
IRType_t<FloatLiteral> GenericEmitter::makeFloatLiteral(FloatLiteral *literal) {
  return llvm::ConstantFP::get(emitQualType(literal->getType()), literal->getValue());
}
llvm::Value* GenericEmitter::makeAddOp(QualType type, llvm::Value *lhs, llvm::Value *rhs) {
  if (isa<IntType>(type.getType()))
    return builder.CreateAdd(lhs, rhs, "", false, false);
  else if (isa<FloatType>(type.getType()))
    return builder.CreateFAdd(lhs, rhs, "");
  return nullptr;
}
llvm::Value* GenericEmitter::makeSubOp(QualType type, llvm::Value *lhs, llvm::Value *rhs) {
  if (isa<IntType>(type.getType()))
    return builder.CreateSub(lhs, rhs, "", false, false);
  else if (isa<FloatType>(type.getType()))
    return builder.CreateFSub(lhs, rhs, "");
  return nullptr;
}
llvm::Value* GenericEmitter::makeMulOp(QualType type, llvm::Value *lhs, llvm::Value *rhs) {
  if (isa<IntType>(type.getType()))
    return builder.CreateMul(lhs, rhs, "", false, false);
  else if (isa<FloatType>(type.getType()))
    return builder.CreateFMul(lhs, rhs, "");
  return nullptr;
}
llvm::Value* GenericEmitter::makeDivOp(QualType type, llvm::Value *lhs, llvm::Value *rhs) {
  if (auto it = dyn_cast<IntType>(type.getType())) {
    if (it->getSign() == IntType::Signed)
      return builder.CreateSDiv(lhs, rhs, "");
    return builder.CreateUDiv(lhs, rhs, "");
  } else if (isa<FloatType>(type.getType()))
    return builder.CreateFDiv(lhs, rhs, "");
  return nullptr;
}
llvm::Value* GenericEmitter::makeLogic(BinaryOperator::Operator kind, llvm::Value *lhs, llvm::Value *rhs) {
  switch (kind) {
  case BinaryOperator::Or:
    return builder.CreateOr(lhs, rhs);
  case BinaryOperator::And:
    return builder.CreateAnd(lhs, rhs);
  default:
    return nullptr;
  }
}
llvm::Value* GenericEmitter::makeCmp(QualType type, BinaryOperator::Operator kind, llvm::Value *lhs, llvm::Value *rhs) {
  llvm::CmpInst::Predicate predicate;
  bool isInt = isa<IntType>(type.getType()) || isa<BoolType>(type.getType());
  bool isSigned = false;
  if (auto it = dyn_cast<IntType>(type.getType()))
    isSigned = it->getSign() == IntType::Signed;
  switch (kind) {
  case BinaryOperator::Equal:
    predicate = isInt ? llvm::CmpInst::ICMP_EQ : llvm::CmpInst::FCMP_OEQ;
    break;
  case BinaryOperator::NEQ:
    predicate = isInt ? llvm::CmpInst::ICMP_NE : llvm::CmpInst::FCMP_ONE;
    break;
  case BinaryOperator::Less:
    predicate = isInt ? (isSigned ? llvm::CmpInst::ICMP_SLT : llvm::CmpInst::ICMP_ULT) : llvm::CmpInst::FCMP_OLT;
    break;
  case BinaryOperator::Greater:
    predicate = isInt ? (isSigned ? llvm::CmpInst::ICMP_SGT : llvm::CmpInst::ICMP_UGT) : llvm::CmpInst::FCMP_OGT;
    break;
  case BinaryOperator::LEQ:
    predicate = isInt ? (isSigned ? llvm::CmpInst::ICMP_SLE : llvm::CmpInst::ICMP_ULE) : llvm::CmpInst::FCMP_OLE;
    break;
  case BinaryOperator::GEQ:
    predicate = isInt ? (isSigned ? llvm::CmpInst::ICMP_SGE : llvm::CmpInst::ICMP_UGE) : llvm::CmpInst::FCMP_OGE;
    break;
  default:
    return nullptr;
  }
  if (isInt)
    return builder.CreateICmp(predicate, lhs, rhs, "");
  else if (isa<FloatType>(type.getType()))
    return builder.CreateFCmp(predicate, lhs, rhs, "");
  return nullptr;
}
llvm::Value* GenericEmitter::makeLoad(QualType type, llvm::Value *value) {
  return builder.CreateLoad(emitQualType(type.modQuals()), value);
}
llvm::Value* GenericEmitter::makeStore(llvm::Value *ptr, llvm::Value *value) {
  return builder.CreateStore(value, ptr);
}
IRType_t<CastExpr> GenericEmitter::makeCast(Type *dest, Type *source, llvm::Value *subExpr) {
  if (dest == source)
    return subExpr;
  auto IRdest = emitType(dest);
  if (isa<PtrType>(dest) || isa<AddressType>(dest))
    return builder.CreateBitCast(subExpr, IRdest);
  if (isArithmetic(dest) && isArithmetic(source)) {
    switch (dest->classof()) {
    case ASTKind::IntType:
      switch (source->classof()) {
      case ASTKind::IntType: {
        auto *d = static_cast<IntType*>(dest);
        auto *s = static_cast<IntType*>(source);
        if (d->getPrecision() > s->getPrecision()) {
          if (d->isUnsigned())
            return builder.CreateZExt(subExpr, IRdest);
          return s->isSigned() ? builder.CreateSExt(subExpr, IRdest) : builder.CreateZExt(subExpr, IRdest);
        } else if (d->getPrecision() == s->getPrecision())
          return subExpr;
        else
          return builder.CreateTrunc(subExpr, IRdest);
      }
      case ASTKind::FloatType: {
        auto *d = static_cast<IntType*>(dest);
        return d->isSigned() ? builder.CreateFPToSI(subExpr, IRdest) : builder.CreateFPToUI(subExpr, IRdest);
      }
      case ASTKind::BoolType: {
        auto *d = static_cast<IntType*>(dest);
        return d->isSigned() ? builder.CreateSExt(subExpr, IRdest) : builder.CreateZExt(subExpr, IRdest);
      }
      default:
        return nullptr;
      }
    case ASTKind::FloatType:
      switch (source->classof()) {
      case ASTKind::IntType: {
        auto *s = static_cast<IntType*>(source);
        return s->isSigned() ? builder.CreateSIToFP(subExpr, IRdest) : builder.CreateUIToFP(subExpr, IRdest);
      }
      case ASTKind::FloatType: {
        auto *d = static_cast<FloatType*>(dest);
        auto *s = static_cast<FloatType*>(source);
        return d->getPrecision() > s->getPrecision() ? builder.CreateFPExt(subExpr, IRdest) : builder.CreateFPTrunc(subExpr, IRdest);
      }
      case ASTKind::BoolType:
        return builder.CreateUIToFP(subExpr, IRdest);
      default:
        return nullptr;
      }
    case ASTKind::BoolType:
      switch (source->classof()) {
      case ASTKind::IntType:
        return builder.CreateTrunc(subExpr, IRdest);
      case ASTKind::FloatType:
        return builder.CreateFPToUI(subExpr, IRdest);
      default:
        return nullptr;
      }
    default:
      return nullptr;
    }
  }
  return nullptr;
}
IRType_t<BooleanLiteral> GenericEmitter::emitBooleanLiteral(BooleanLiteral *literal) {
  return makeBooleanLiteral(literal);
}
IRType_t<IntegerLiteral> GenericEmitter::emitIntegerLiteral(IntegerLiteral *literal) {
  return makeIntegerLiteral(literal);
}
IRType_t<UIntegerLiteral> GenericEmitter::emitUIntegerLiteral(UIntegerLiteral *literal) {
  return makeIntegerLiteral(literal);
}
IRType_t<FloatLiteral> GenericEmitter::emitFloatLiteral(FloatLiteral *literal) {
  return makeFloatLiteral(literal);
}
IRType_t<ParenExpr> GenericEmitter::emitParenExpr(ParenExpr *expr) {
  return emitExpr(expr->getExpr());
}
IRType_t<UnaryOperator> GenericEmitter::emitUnaryOperator(UnaryOperator *expr) {
  UnaryOperator::Operator op = expr->getOp();
  auto subExpr = emitExpr(expr->getExpr());
  switch (op) {
  case UnaryOperator::Address:
    return subExpr;
  case UnaryOperator::Dereference:
    return subExpr;
  default:
    return nullptr;
  }
}
IRType_t<BinaryOperator> GenericEmitter::emitBinaryOperator(BinaryOperator *expr) {
  BinaryOperator::Operator op = expr->getOp();
  llvm::Value *lhs = emitExpr(expr->getLhs());
  llvm::Value *rhs = emitExpr(expr->getRhs());
  auto type = expr->getType();
  switch (op) {
  case BinaryOperator::Plus:
    return makeAddOp(type, lhs, rhs);
  case BinaryOperator::Minus:
    return makeSubOp(type, lhs, rhs);
  case BinaryOperator::Multiply:
    return makeMulOp(type, lhs, rhs);
  case BinaryOperator::Divide:
    return makeDivOp(type, lhs, rhs);
  case BinaryOperator::Equal:
  case BinaryOperator::NEQ:
  case BinaryOperator::Less:
  case BinaryOperator::Greater:
  case BinaryOperator::LEQ:
  case BinaryOperator::GEQ:
    return makeCmp(type, op, lhs, rhs);
  case BinaryOperator::And:
  case BinaryOperator::Or:
    return makeLogic(op, lhs, rhs);
  case BinaryOperator::Assign:
    makeStore(lhs, rhs);
    return lhs;
  default:
    return nullptr;
  }
}
IRType_t<DeclRefExpr> GenericEmitter::emitDeclRefExpr(DeclRefExpr *expr) {
  auto decl = expr->getDecl().data();
  if (auto vd = dyn_cast<VariableDecl>(decl)) {
    auto alloca = get(vd);
    assert(alloca);
    return alloca;
  }
  return nullptr;
}
IRType_t<MemberExpr> GenericEmitter::emitMemberExpr(MemberExpr *expr) {
  auto ptr = emitExpr(expr->getOwner());
  int indx = -1;
  if (auto re = dyn_cast<DeclRefExpr>(expr->getMember().data()))
    if (auto md = dyn_cast<MemberDecl>(re->getDecl().data()))
      indx = md->getIndex();
  assert(indx > 0);
  auto index = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), indx, true);
  return builder.CreateGEP(emitQualType(expr->getOwner()->getType().modQuals()), ptr, index);
}
IRType_t<CallExpr> GenericEmitter::emitCallExpr(CallExpr *expr) {
  if (auto callee = dynamic_cast<DeclRefExpr*>(expr->getCallee())) {
    llvm::Function *function = module.getFunction(callee->getIdentifier());
    std::vector<llvm::Value*> arguments(expr->getArgs().size());
    for (auto [i, arg] : tlang::enumerate(expr->getArgs())) {
      arguments[i] = emitExpr(arg);
      if (!arguments[i])
        throw(std::runtime_error("Invalid call expr"));
    }
    return builder.CreateCall(function, arguments);
  }
  return nullptr;
}
IRType_t<ArrayExpr> GenericEmitter::emitArrayExpr(ArrayExpr *expr) {
  llvm::Type *type;
  if (isa<ArrayType>(expr->getArray()->getType().getType()))
    type = emitQualType(expr->getArray()->getType().modQuals());
  else
    type = emitQualType(expr->getType().modQuals());
  auto array = emitExpr(expr->getArray());
  auto index = emitExpr(expr->getIndex()[0]);

  if (isa<ArrayType>(expr->getArray()->getType().getType())) {
    std::vector<llvm::Value*> indxs;
    indxs.push_back(llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0, true));
    indxs.push_back(index);
    return builder.CreateGEP(type, array, indxs);
  }
  return builder.CreateGEP(type, array, index);
}
IRType_t<CastExpr> GenericEmitter::emitCastExpr(CastExpr *expr) {
  auto subExpr = emitExpr(expr->getExpr());
  auto &ft = expr->getType();
  auto st = expr->getExpr()->getType();
  if (ft.getType() == st.getType() && !ft.isReference() && st.isReference())
    return makeLoad(ft, subExpr);
  return makeCast(expr->getType().getCanonicalType().getType(), expr->getExpr()->getType().getCanonicalType().getType(), subExpr);
}
IRType_t<ImplicitCastExpr> GenericEmitter::emitImplicitCastExpr(ImplicitCastExpr *expr) {
  auto subExpr = emitExpr(expr->getExpr());
  auto &ft = expr->getType();
  auto st = expr->getExpr()->getType();
  if (ft.getType() == st.getType() && !ft.isReference() && st.isReference())
    return makeLoad(ft, subExpr);
  return makeCast(expr->getType().getCanonicalType().getType(), expr->getExpr()->getType().getCanonicalType().getType(), subExpr);
}
IRType_t<TernaryOperator> GenericEmitter::emitTernaryOperator(TernaryOperator *expr) {
  auto condition = emitExpr(expr->getCondition());
  auto tval = emitExpr(expr->getLhs());
  auto fval = emitExpr(expr->getRhs());
  return builder.CreateSelect(condition, tval, fval);
}
IRType_t<DimExpr> GenericEmitter::emitDimExpr(DimExpr *expr) {
  return makeIntegerLiteral(ASTApi { ast_context }.CreateLiteral((int64_t) 1));
}
IRType_t<IdExpr> GenericEmitter::emitIdExpr(IdExpr *expr) {
  return makeIntegerLiteral(ASTApi { ast_context }.CreateLiteral((int64_t) 0));
}
}
