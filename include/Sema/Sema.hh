#ifndef __SEMA_SEMA_HH__
#define __SEMA_SEMA_HH__

#include <Sema/FirstSemaPass.hh>
#include <Sema/SecondSemaPass.hh>
#include "AST/Include.hh"
#include "Parent.hh"

namespace tlang::sema {
struct Sema {
  Sema(ASTContext &context) :
      context(context), sctx(*context) {
    ScopeFillerAST { sctx }.traverseModuleDecl(*context);
  }
  void analyze() {
    FirstSemaPass(context, sctx);
    SecondSemaPass(context, sctx);
  }
  ASTContext &context;
  ScopeContext sctx;
};
}
#endif
