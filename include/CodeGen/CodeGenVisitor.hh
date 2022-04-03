#ifndef CODEGEN_CODEGENVISITOR_HH
#define CODEGEN_CODEGENVISITOR_HH

#include "Traits.hh"
#include <AST/Attr.hh>
#include <AST/Decl.hh>
#include <AST/Expr.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>

namespace tlang::codegen {
template <typename Derived> class CodeGenVisitor {
public:
  template <typename T> using return_t = IRType_t<T>;
  inline Derived &getDerived() {
    return *static_cast<Derived *>(this);
  }
  return_t<UnitDecl> emitUnitDecl(UnitDecl *node) {
    return_t<UnitDecl> last{};
    for (auto &symbol : *static_cast<UnitContext *>(node))
      last = emitDecl(*symbol);
    return last;
  }
  return_t<ModuleDecl> emitModuleDecl(ModuleDecl *node) {
    return_t<ModuleDecl> last{};
    for (auto &symbol : *static_cast<DeclContext *>(node))
      last = emitDecl(*symbol);
    return last;
  }
  return_t<CompoundStmt> emitCompoundStmt(CompoundStmt *node) {
    return_t<CompoundStmt> last{};
    auto &stmts = node->getStmts();
    for (auto &stmt : stmts)
      last = emitStmt(node);
    return last;
  }
#define VISIT_UNITDECL 0
#define VISIT_MODULEDECL 0
#define VISIT_COMPOUNDSTMT 0
#define NO_ABSTRACT
#define AST_MACRO(BASE, PARENT)                                                                                        \
  CodeGenTraits_t<BASE> emit##BASE(BASE *node) {                                                                       \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>

  return_t<Stmt> emitStmt(Stmt *node) {
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
  return_t<Decl> emitDecl(Decl *node) {
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
  return_t<Type> emitType(Type *node) {
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
};
} // namespace tlang::codegen

#endif
