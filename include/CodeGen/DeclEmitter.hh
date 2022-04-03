#ifndef CODEGEN_DECLEMITTER_HH
#define CODEGEN_DECLEMITTER_HH

#include <AST/Attr.hh>
#include <AST/Decl.hh>
#include <AST/Expr.hh>
#include <AST/Stmt.hh>
#include <AST/Type.hh>
#include <CodeGen/Traits.hh>

namespace tlang::codegen {
namespace impl {
template <typename Derived>
class DeclEmitter {
public:
  inline Derived& getDerived() {
    return *static_cast<Derived*>(this);
  }
#define NO_ABSTRACT
#define DECL(BASE, PARENT)                                                                                             \
  IRType_t<BASE> emit##BASE(BASE *node) {                                                                              \
    return nullptr;                                                                                                    \
  }
#include <AST/Nodes.inc>
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
};
} // namespace impl
template <typename TypeEmitter, typename StmtEmitter>
class DeclEmitter: public impl::DeclEmitter<DeclEmitter<TypeEmitter, StmtEmitter>> {
public:

private:
  TypeEmitter &type_emitter;
  StmtEmitter &stmt_emitter;
};
} // namespace tlang::codegen

#endif
