#ifndef __AST_CONTEXT_HH__
#define __AST_CONTEXT_HH__

#include "ast_node.hh"

namespace _astnp_ {
struct ASTContext {
  std::unique_ptr<ModuleDecl> translation_unit;
  static ASTContext create(const Identifier &id) {
    ASTContext ctx;
    ctx.translation_unit = std::make_unique<ModuleDecl>();
    ctx.translation_unit->getIdentifier() = id;
    return ctx;
  }
  ModuleDecl* operator*() const {
    return translation_unit.get();
  }
};
}
#endif
