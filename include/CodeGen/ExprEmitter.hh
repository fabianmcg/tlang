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
template <typename Derived>
class ExprEmitterVisitor {
public:
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }
#define NO_ABSTRACT
#define EXPR(BASE, PARENT)                                                                                             \
  IRType_t<BASE> emit##BASE(BASE *node) {                                                                              \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  IRType_t<Expr> emitExpr(Expr *node) {
    if (!node)
      return nullptr;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define EXPR(BASE, PARENT)                                                                                             \
  case ASTKind::BASE:                                                                                                  \
    return derived.emit##BASE(static_cast<BASE *>(node));
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
  inline IRType_t<QualType> emitQualType(QualType &type) {
    return typeEmitter.emitQualType(type);
  }
  llvm::Value* makeAddOp(QualType type, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeSubOp(QualType type, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeMulOp(QualType type, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeDivOp(QualType type, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeCmp(QualType type, BinaryOperator::Operator kind, llvm::Value *lhs, llvm::Value *rhs);
  llvm::Value* makeLoad();
  IRType_t<BooleanLiteral> makeBooleanLiteral(BooleanLiteral *literal);
  IRType_t<IntegerLiteral> makeIntegerLiteral(IntegerLiteral *literal);
  IRType_t<UIntegerLiteral> makeIntegerLiteral(UIntegerLiteral *literal);
  IRType_t<FloatLiteral> makeFloatLiteral(FloatLiteral *literal);
protected:
  TypeEmitter &typeEmitter;
};
class ExprEmitterVisitor: public ExprEmitter, public impl::ExprEmitterVisitor<ExprEmitterVisitor> {
public:
  IRType_t<BooleanLiteral> emitBooleanLiteral(BooleanLiteral *literal);
  IRType_t<IntegerLiteral> emitIntegerLiteral(IntegerLiteral *literal);
  IRType_t<UIntegerLiteral> emitUIntegerLiteral(UIntegerLiteral *literal);
  IRType_t<FloatLiteral> emitFloatLiteral(FloatLiteral *literal);
  IRType_t<ParenExpr> emitParenExpr(ParenExpr *expr);
  IRType_t<UnaryOperator> emitUnaryOperator(UnaryOperator *expr);
  IRType_t<BinaryOperator> emitBinaryOperator(BinaryOperator *expr);
  IRType_t<DeclRefExpr> emitDeclRefExpr(DeclRefExpr *expr);
  IRType_t<MemberExpr> emitDeclMemberExpr(MemberExpr *expr);
  IRType_t<CallExpr> emitCallExpr(CallExpr *expr);
//  IRType_t<MemberCallExpr> emitMemberCallExpr(MemberCallExpr *expr);
  IRType_t<ArrayExpr> emitArrayExpr(ArrayExpr *expr);
  IRType_t<CastExpr> emitCastExpr(CastExpr *expr);

  using tlang::codegen::ExprEmitter::ExprEmitter;
};
} // namespace tlang::codegen
#endif
