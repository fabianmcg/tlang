#ifndef CODEGEN_EXPREMITTER_HH
#define CODEGEN_EXPREMITTER_HH

#include <AST/Attr.hh>
#include <AST/Decl.hh>
#include <AST/Expr.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>
#include <CodeGen/CodeEmitterContext.hh>
#include <CodeGen/Emitter.hh>
#include <CodeGen/Traits.hh>
#include <CodeGen/TypeEmitter.hh>

namespace tlang::codegen {
namespace impl {
template <typename T>
struct ExprEmitterReturn {
  ExprEmitterReturn(nullptr_t) {
  }
  template <typename S>
  ExprEmitterReturn(S *value, bool requiresLoad = false) :
      value(value), requiresLoad(requiresLoad) {
  }
  T value { };
  bool requiresLoad { };
};
template <typename Derived>
class ExprEmitterVisitor {
public:
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }
#define NO_ABSTRACT
#define EXPR(BASE, PARENT)                                                                                             \
  ExprEmitterReturn<IRType_t<BASE>> emit##BASE##Impl(BASE *node) {                                                     \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  ExprEmitterReturn<IRType_t<Expr>> emitExprImpl(Expr *node) {
    if (!node)
      return nullptr;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define EXPR(BASE, PARENT)                                                                                             \
  case ASTKind::BASE:                                                                                                  \
    return derived.emit##BASE##Impl(static_cast<BASE *>(node));
#include <AST/Nodes.inc>
    default:
      return nullptr;
    }
  }
};
} // namespace impl
class ExprEmitter: public CodeEmitterContext, public EmitterTable {
public:
  ExprEmitter(Emitter &emitter, TypeEmitter &type_emitter);
  inline IRType_t<Type> emitType(Type *type) {
    return typeEmitter.emitType(type);
  }
  inline IRType_t<QualType> emitQualType(QualType type) {
    return typeEmitter.emitQualType(type);
  }
  llvm::Value* makeAddOp(QualType type, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeSubOp(QualType type, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeMulOp(QualType type, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeDivOp(QualType type, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeCmp(QualType type, BinaryOperator::Operator kind, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeLoad(QualType type, llvm::Value *value);
  llvm::Value* makeStore(llvm::Value *value, llvm::Value *ptr);
  IRType_t<BooleanLiteral> makeBooleanLiteral(BooleanLiteral *literal);
  IRType_t<IntegerLiteral> makeIntegerLiteral(IntegerLiteral *literal);
  IRType_t<UIntegerLiteral> makeIntegerLiteral(UIntegerLiteral *literal);
  IRType_t<FloatLiteral> makeFloatLiteral(FloatLiteral *literal);
  IRType_t<CastExpr> makeCast(Type *dest, Type *source, llvm::Value *subExpr);

protected:
  TypeEmitter &typeEmitter;
};
class ExprEmitterVisitor: public ExprEmitter, public impl::ExprEmitterVisitor<ExprEmitterVisitor> {
public:
  template <typename T>
  using return_t = impl::ExprEmitterReturn<IRType_t<T>>;
  return_t<BooleanLiteral> emitBooleanLiteralImpl(BooleanLiteral *literal);
  return_t<IntegerLiteral> emitIntegerLiteralImpl(IntegerLiteral *literal);
  return_t<UIntegerLiteral> emitUIntegerLiteralImpl(UIntegerLiteral *literal);
  return_t<FloatLiteral> emitFloatLiteralImpl(FloatLiteral *literal);
  return_t<ParenExpr> emitParenExprImpl(ParenExpr *expr);
  return_t<UnaryOperator> emitUnaryOperatorImpl(UnaryOperator *expr);
  return_t<BinaryOperator> emitBinaryOperatorImpl(BinaryOperator *expr);
  return_t<DeclRefExpr> emitDeclRefExprImpl(DeclRefExpr *expr);
  return_t<MemberExpr> emitMemberExprImpl(MemberExpr *expr);
  return_t<CallExpr> emitCallExprImpl(CallExpr *expr);
  //  return_t<MemberCallExpr> emitMemberCallExprImpl(MemberCallExpr *expr);
  return_t<ArrayExpr> emitArrayExprImpl(ArrayExpr *expr);
  return_t<CastExpr> emitCastExprImpl(CastExpr *expr);
  return_t<ImplicitCastExpr> emitImplicitCastExprImpl(ImplicitCastExpr *expr);
  return_t<Expr> emitExprAndLoad(Expr *node, bool loadValue);
  IRType_t<Expr> emitExpr(Expr *node) {
    return emitExprAndLoad(node, true).value;
  }
  using tlang::codegen::ExprEmitter::ExprEmitter;
};
} // namespace tlang::codegen
#endif
