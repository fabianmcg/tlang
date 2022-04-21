#include <AST/Visitors/ASTVisitor.hh>
#include <CodeGen/ExprEmitter.hh>
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
#include <llvm/IR/IntrinsicsNVPTX.h>
#include <Support/Enumerate.hh>

namespace tlang::codegen {
ExprEmitter::ExprEmitter(Emitter &emitter, TypeEmitter &type_emitter) :
    CodeEmitterContext(static_cast<CodeEmitterContext&>(emitter)), EmitterTable(emitter), typeEmitter(type_emitter) {
}
IRType_t<BooleanLiteral> ExprEmitter::makeBooleanLiteral(BooleanLiteral *literal) {
  uint64_t value = static_cast<uint64_t>(literal->getValue());
  return llvm::ConstantInt::get(emitQualType(literal->getType()), value, false);
}
IRType_t<IntegerLiteral> ExprEmitter::makeIntegerLiteral(IntegerLiteral *literal) {
  uint64_t value = static_cast<uint64_t>(literal->getValue());
  return llvm::ConstantInt::get(emitQualType(literal->getType()), value, true);
}
IRType_t<UIntegerLiteral> ExprEmitter::makeIntegerLiteral(UIntegerLiteral *literal) {
  uint64_t value = static_cast<uint64_t>(literal->getValue());
  return llvm::ConstantInt::get(emitQualType(literal->getType()), value, false);
}
IRType_t<FloatLiteral> ExprEmitter::makeFloatLiteral(FloatLiteral *literal) {
  return llvm::ConstantFP::get(emitQualType(literal->getType()), literal->getValue());
}
llvm::Value* ExprEmitter::makeAddOp(QualType type, llvm::Value *lhs, llvm::Value *rhs) {
  if (isa<IntType>(type.getType()))
    return builder.CreateAdd(lhs, rhs, "", false, false);
  else if (isa<FloatType>(type.getType()))
    return builder.CreateFAdd(lhs, rhs, "");
  return nullptr;
}
llvm::Value* ExprEmitter::makeSubOp(QualType type, llvm::Value *lhs, llvm::Value *rhs) {
  if (isa<IntType>(type.getType()))
    return builder.CreateSub(lhs, rhs, "", false, false);
  else if (isa<FloatType>(type.getType()))
    return builder.CreateFSub(lhs, rhs, "");
  return nullptr;
}
llvm::Value* ExprEmitter::makeMulOp(QualType type, llvm::Value *lhs, llvm::Value *rhs) {
  if (isa<IntType>(type.getType()))
    return builder.CreateMul(lhs, rhs, "", false, false);
  else if (isa<FloatType>(type.getType()))
    return builder.CreateFMul(lhs, rhs, "");
  return nullptr;
}
llvm::Value* ExprEmitter::makeDivOp(QualType type, llvm::Value *lhs, llvm::Value *rhs) {
  if (auto it = dyn_cast<IntType>(type.getType())) {
    if (it->getSign() == IntType::Signed)
      return builder.CreateSDiv(lhs, rhs, "");
    return builder.CreateUDiv(lhs, rhs, "");
  } else if (isa<FloatType>(type.getType()))
    return builder.CreateFDiv(lhs, rhs, "");
  return nullptr;
}
llvm::Value* ExprEmitter::makeLogic(BinaryOperator::Operator kind, llvm::Value *lhs, llvm::Value *rhs) {
  switch (kind) {
  case BinaryOperator::Or:
    return builder.CreateOr(lhs, rhs);
  case BinaryOperator::And:
    return builder.CreateAnd(lhs, rhs);
  default:
    return nullptr;
  }
}
llvm::Value* ExprEmitter::makeCmp(QualType type, BinaryOperator::Operator kind, llvm::Value *lhs, llvm::Value *rhs) {
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
llvm::Value* ExprEmitter::makeLoad(QualType type, llvm::Value *value) {
  return builder.CreateLoad(emitQualType(type.modQuals()), value);
}
llvm::Value* ExprEmitter::makeStore(llvm::Value *ptr, llvm::Value *value) {
  return builder.CreateStore(value, ptr);
}
IRType_t<CastExpr> ExprEmitter::makeCast(Type *dest, Type *source, llvm::Value *subExpr) {
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
template <typename T>
using ExprReturn = ExprEmitterVisitor::return_t<T>;
ExprReturn<Expr> ExprEmitterVisitor::emitExprAndLoad(Expr *node, bool loadValue) {
  ExprReturn<Expr> value = emitExprImpl(node);
  if (loadValue && value.requiresLoad)
    return makeLoad(node->getType(), value.value);
  return value;
}
ExprReturn<BooleanLiteral> ExprEmitterVisitor::emitBooleanLiteralImpl(BooleanLiteral *literal) {
  return makeBooleanLiteral(literal);
}
ExprReturn<IntegerLiteral> ExprEmitterVisitor::emitIntegerLiteralImpl(IntegerLiteral *literal) {
  return makeIntegerLiteral(literal);
}
ExprReturn<UIntegerLiteral> ExprEmitterVisitor::emitUIntegerLiteralImpl(UIntegerLiteral *literal) {
  return makeIntegerLiteral(literal);
}
ExprReturn<FloatLiteral> ExprEmitterVisitor::emitFloatLiteralImpl(FloatLiteral *literal) {
  return makeFloatLiteral(literal);
}
ExprReturn<ParenExpr> ExprEmitterVisitor::emitParenExprImpl(ParenExpr *expr) {
  return emitExprImpl(expr->getExpr());
}
ExprReturn<UnaryOperator> ExprEmitterVisitor::emitUnaryOperatorImpl(UnaryOperator *expr) {
  UnaryOperator::Operator op = expr->getOp();
  auto subExpr = emitExprImpl(expr->getExpr());
  switch (op) {
  case UnaryOperator::Address:
    return {subExpr.value, false};
  case UnaryOperator::Dereference:
    return {subExpr.requiresLoad? makeLoad(expr->getExpr()->getType(), subExpr.value) : subExpr.value, true};
  default:
    return nullptr;
  }
}
ExprReturn<BinaryOperator> ExprEmitterVisitor::emitBinaryOperatorImpl(BinaryOperator *expr) {
  BinaryOperator::Operator op = expr->getOp();
  llvm::Value *lhs = emitExprAndLoad(expr->getLhs(), op != BinaryOperator::Assign).value;
  llvm::Value *rhs = emitExprAndLoad(expr->getRhs(), true).value;
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
    return {lhs, true};
  default:
    return nullptr;
  }
}
ExprReturn<DeclRefExpr> ExprEmitterVisitor::emitDeclRefExprImpl(DeclRefExpr *expr) {
  auto decl = expr->getDecl().data();
  if (auto vd = dyn_cast<VariableDecl>(decl)) {
    auto alloca = get(vd);
    assert(alloca);
    return {alloca, true};
  }
  return nullptr;
}
ExprReturn<MemberExpr> ExprEmitterVisitor::emitMemberExprImpl(MemberExpr *expr) {
  auto ptr = emitExprImpl(expr->getOwner()).value;
  int indx = -1;
  if (auto re = dyn_cast<DeclRefExpr>(expr->getMember().data()))
    if (auto md = dyn_cast<MemberDecl>(re->getDecl().data()))
      indx = md->getIndex();
  assert(indx > 0);
  auto index = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), indx, true);
  return {builder.CreateGEP(emitQualType(expr->getOwner()->getType().modQuals()), ptr, index), true};
}
ExprReturn<CallExpr> ExprEmitterVisitor::emitCallExprImpl(CallExpr *expr) {
  if (auto callee = dynamic_cast<DeclRefExpr*>(expr->getCallee())) {
    llvm::Function *function = module.getFunction(callee->getIdentifier());
    std::vector<llvm::Value*> arguments(expr->getArgs().size());
    for (auto [i, arg] : tlang::enumerate(expr->getArgs())) {
      arguments[i] = emitExprAndLoad(arg, true).value;
      if (!arguments.back())
        throw(std::runtime_error("Invalid call expr"));
    }
    return builder.CreateCall(function, arguments);
  }
  return nullptr;
}
ExprReturn<ArrayExpr> ExprEmitterVisitor::emitArrayExprImpl(ArrayExpr *expr) {
  auto type = emitQualType(expr->getType().modQuals());
  auto array = emitExprAndLoad(expr->getArray(), true).value;
  auto index = emitExprAndLoad(expr->getIndex()[0], true).value;
  return {builder.CreateGEP(type, array, index), true};
}
ExprReturn<CastExpr> ExprEmitterVisitor::emitCastExprImpl(CastExpr *expr) {
  auto subExpr = emitExprAndLoad(expr->getExpr(), true).value;
  return makeCast(expr->getType().getCanonicalType().getType(), expr->getExpr()->getType().getCanonicalType().getType(), subExpr);
}
ExprReturn<ImplicitCastExpr> ExprEmitterVisitor::emitImplicitCastExprImpl(ImplicitCastExpr *expr) {
  expr->dump(std::cerr);
  auto subExpr = emitExprAndLoad(expr->getExpr(), true).value;
  return makeCast(expr->getType().getCanonicalType().getType(), expr->getExpr()->getType().getCanonicalType().getType(), subExpr);
}
ExprReturn<TernaryOperator> ExprEmitterVisitor::emitTernaryOperatorImpl(TernaryOperator *expr) {
  auto condition = emitExprAndLoad(expr->getCondition(), true).value;
  auto tval = emitExprAndLoad(expr->getLhs(), true).value;
  auto fval = emitExprAndLoad(expr->getRhs(), true).value;
  return builder.CreateSelect(condition, tval, fval);
}

ExprReturn<IdExpr> ExprEmitterVisitor::emitIdExprImpl(IdExpr *expr) {
  std::vector<int> nums;
//  for (auto e : expr->getIndex()) {
//    if (auto il = dyn_cast<IntegerLiteral>(e)) {
//      nums.push_back(il->getValue());
//    } else
//      throw(std::runtime_error("Invalid IDX"));
//  }
  if (nums.size() != 2)
    throw(std::runtime_error("Invalid IDX"));
  switch (nums[1]) {
  case 0:
    return
        nums[0] == 0 ?
            builder.CreateIntrinsic(llvm::Intrinsic::nvvm_read_ptx_sreg_ctaid_x, { }, { }) :
            builder.CreateIntrinsic(llvm::Intrinsic::nvvm_read_ptx_sreg_tid_x, { }, { });
  case 1:
    return
        nums[0] == 0 ?
            builder.CreateIntrinsic(llvm::Intrinsic::nvvm_read_ptx_sreg_ctaid_y, { }, { }) :
            builder.CreateIntrinsic(llvm::Intrinsic::nvvm_read_ptx_sreg_tid_y, { }, { });
  case 2:
    return
        nums[0] == 0 ?
            builder.CreateIntrinsic(llvm::Intrinsic::nvvm_read_ptx_sreg_ctaid_z, { }, { }) :
            builder.CreateIntrinsic(llvm::Intrinsic::nvvm_read_ptx_sreg_tid_z, { }, { });
  default:
    return nullptr;
  }
}
}
