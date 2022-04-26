#ifndef IO_STRINGEMITTER_HH
#define IO_STRINGEMITTER_HH

#include "AST/Attr.hh"
#include "AST/Decl.hh"
#include "AST/Expr.hh"
#include "AST/Type.hh"
#include "AST/Stmt.hh"
#include <Support/Format.hh>
#include <Support/Enumerate.hh>

namespace tlang {
template <typename Derived>
class StringEmitterVisitor {
public:
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }
  /******************************************************************************
   * Emit types
   ******************************************************************************/
#define NO_ABSTRACT
#define TYPE(BASE, PARENT)                                                                                             \
  std::string emit##BASE(BASE *node) {                                                                                 \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  /******************************************************************************
   * Emit declarations
   ******************************************************************************/
#define NO_ABSTRACT
#define DECL(BASE, PARENT)                                                                                             \
  std::string emit##BASE(BASE *node) {                                                                                 \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  /******************************************************************************
   * Emit statements
   ******************************************************************************/
#define NO_ABSTRACT
#define STMT(BASE, PARENT)                                                                                             \
  std::string emit##BASE(BASE *node) {                                                                                 \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
  /******************************************************************************
   * Dynamic dispatchers
   ******************************************************************************/
  std::string emitType(Type *node) {
    if (!node)
      return nullptr;
    auto &derived = getDerived();
    switch (node->classof()) {
#define NO_ABSTRACT
#define TYPE(BASE, PARENT)                                                                                             \
  case ASTKind::BASE:                                                                                                  \
    return derived.emit##BASE(static_cast<BASE *>(node));
#include <AST/Nodes.inc>
    default:
      return nullptr;
    }
  }
  std::string emitDecl(Decl *node) {
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
  std::string emitExpr(Expr *node) {
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
  std::string emitStmt(Stmt *node) {
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
}

#endif
