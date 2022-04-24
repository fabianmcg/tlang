#ifndef CODEGEN_EMITTERVISITOR_HH
#define CODEGEN_EMITTERVISITOR_HH

#include <AST/Type.hh>
#include <CodeGen/Traits.hh>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

namespace tlang::codegen {
namespace impl {
template <typename Derived>
class EmitterVisitor {
public:
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }
  /******************************************************************************
   * Emit types
   ******************************************************************************/
#define NO_ABSTRACT
#define TYPE(BASE, PARENT)                                                                                             \
  IRType_t<BASE> emit##BASE(BASE *node, int as = 0) {                                                                  \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  /******************************************************************************
   * Emit declarations
   ******************************************************************************/
#define NO_ABSTRACT
#define DECL(BASE, PARENT)                                                                                             \
  IRType_t<BASE> emit##BASE(BASE *node) {                                                                              \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  /******************************************************************************
   * Emit statements
   ******************************************************************************/
#define NO_ABSTRACT
#define STMT(BASE, PARENT)                                                                                             \
  IRType_t<BASE> emit##BASE(BASE *node) {                                                                              \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  /******************************************************************************
   * Dynamic dispatchers
   ******************************************************************************/
  IRType_t<Type> emitType(Type *node, int as = 0) {
    if (!node)
      return nullptr;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define TYPE(BASE, PARENT)                                                                                             \
  case ASTKind::BASE:                                                                                                  \
    return derived.emit##BASE(static_cast<BASE *>(node), as);
#include <AST/Nodes.inc>
    default:
      return nullptr;
    }
  }
  IRType_t<Decl> emitDecl(Decl *node) {
    if (!node)
      return nullptr;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define DECL(BASE, PARENT)                                                                                             \
  case ASTKind::BASE:                                                                                                  \
    return derived.emit##BASE(static_cast<BASE *>(node));
#include <AST/Nodes.inc>
    default:
      return nullptr;
    }
  }
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
  IRType_t<Stmt> emitStmt(Stmt *node) {
    if (!node)
      return nullptr;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define STMT(BASE, PARENT)                                                                                             \
  case ASTKind::BASE:                                                                                                  \
    return derived.emit##BASE(static_cast<BASE *>(node));
#include <AST/Nodes.inc>
    default:
      return nullptr;
    }
  }
};
} // namespace impl
} // namespace tlang::codegen

#endif
