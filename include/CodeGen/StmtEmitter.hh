#ifndef CODEGEN_STMTEMITTER_HH
#define CODEGEN_STMTEMITTER_HH

#include <AST/Attr.hh>
#include <AST/Decl.hh>
#include <AST/Expr.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>
#include <CodeGen/CodeEmitterContext.hh>
#include <CodeGen/Emitter.hh>
#include <CodeGen/ExprEmitter.hh>
#include <CodeGen/Traits.hh>
#include <CodeGen/TypeEmitter.hh>

namespace tlang::codegen {
namespace impl {
template <typename Derived>
class StmtEmitterVisitor {
public:
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }
#define NO_ABSTRACT
#define EXPR(BASE, PARENT)
#define STMT(BASE, PARENT)                                                                                             \
  IRType_t<BASE> emit##BASE(BASE *node) {                                                                              \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  IRType_t<Expr> emitExpr(Expr *node) {
    return nullptr;
  }
  IRType_t<Stmt> emitStmt(Stmt *node) {
    if (!node)
      return nullptr;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define EXPR(BASE, PARENT)
#define STMT(BASE, PARENT)                                                                                             \
  case ASTKind::BASE:                                                                                                  \
    return derived.emit##BASE(static_cast<BASE *>(node));
#include <AST/Nodes.inc>
    case ASTKind::Expr:
      return derived.emitExpr(static_cast<Expr*>(node));
    default:
      return nullptr;
    }
  }
};
} // namespace impl
class StmtEmitter: public CodeEmitterContext, public EmitterTable {
public:
  StmtEmitter(Emitter &emitter, TypeEmitter &type_emitter, ExprEmitterVisitor &expr_emitter);
  inline IRType_t<QualType> emitQualType(QualType &type) {
    return typeEmitter.emitQualType(type);
  }
protected:
  TypeEmitter &typeEmitter;
  ExprEmitterVisitor &exprEmitter;
};
class StmtEmitterVisitor: public StmtEmitter, public impl::StmtEmitterVisitor<StmtEmitterVisitor> {
public:
  using tlang::codegen::StmtEmitter::StmtEmitter;
  inline IRType_t<Expr> emitExpr(Expr *node) {
    return exprEmitter.emitExpr(node);
  }
};
} // namespace tlang::codegen
#endif
