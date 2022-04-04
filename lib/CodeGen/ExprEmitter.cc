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
llvm::Value* ExprEmitter::makeCmp(QualType type, BinaryOperator::Operator kind, llvm::Value *lhs, llvm::Value *rhs) {
  llvm::CmpInst::Predicate predicate;
  bool isInt = isa<IntType>(type.getType());
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

IRType_t<BooleanLiteral> ExprEmitterVisitor::emitBooleanLiteral(BooleanLiteral *literal) {
  return makeBooleanLiteral(literal);
}
IRType_t<IntegerLiteral> ExprEmitterVisitor::emitIntegerLiteral(IntegerLiteral *literal) {
  return makeIntegerLiteral(literal);
}
IRType_t<UIntegerLiteral> ExprEmitterVisitor::emitUIntegerLiteral(UIntegerLiteral *literal) {
  return makeIntegerLiteral(literal);
}
IRType_t<FloatLiteral> ExprEmitterVisitor::emitFloatLiteral(FloatLiteral *literal) {
  return makeFloatLiteral(literal);
}
IRType_t<ParenExpr> ExprEmitterVisitor::emitParenExpr(ParenExpr *expr) {
  return emitExpr(expr->getExpr());
}
IRType_t<UnaryOperator> ExprEmitterVisitor::emitUnaryOperator(UnaryOperator *expr) {
  return nullptr;
}
IRType_t<BinaryOperator> ExprEmitterVisitor::emitBinaryOperator(BinaryOperator *expr) {
  BinaryOperator::Operator op = expr->getOp();
  if (op == BinaryOperator::Assign) {
    llvm::Value *lhs = emitExpr(expr->getLhs());
    llvm::Value *rhs = emitExpr(expr->getRhs());
  }
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
  default:
    return nullptr;
  }
}
IRType_t<DeclRefExpr> ExprEmitterVisitor::emitDeclRefExpr(DeclRefExpr *expr) {
  return nullptr;
}
IRType_t<MemberExpr> ExprEmitterVisitor::emitDeclMemberExpr(MemberExpr *expr) {
  return nullptr;
}
IRType_t<CallExpr> ExprEmitterVisitor::emitCallExpr(CallExpr *expr) {
  return nullptr;
}
IRType_t<ArrayExpr> ExprEmitterVisitor::emitArrayExpr(ArrayExpr *expr) {
  auto type = emitQualType(expr->getType());
  auto array = emitExpr(expr->getArray());
  auto index = emitExpr(expr->getIndex()[0]);
  builder.CreateGEP(type, array, index);
  return nullptr;
}
IRType_t<CastExpr> ExprEmitterVisitor::emitCastExpr(CastExpr *expr) {
  return nullptr;
}
}
