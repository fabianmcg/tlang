#ifndef __AST_ATSCONTEXT_HH__
#define __AST_ASTCONTEXT_HH__

#include "Common/Macros.hh"
#include "ASTCommon.hh"

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
  ASTContext& operator=(std::unique_ptr<ModuleDecl> &&module) {
    translation_unit = std::move(module);
    return *this;
  }
};
}
#endif
