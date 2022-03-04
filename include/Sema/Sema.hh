#ifndef __SEMA_SEMA_HH__
#define __SEMA_SEMA_HH__

#include <Sema/FirstSemaPass.hh>
#include <Sema/SecondSemaPass.hh>
#include "AST/Include.hh"
#include "Parent.hh"
#include "SymbolTablePass.hh"

namespace tlang::sema {
struct Sema {
  Sema(ASTContext &context) :
      context(context) {
    SymbolTablePass(context);
  }
  void analyze() {
  }
  ASTContext &context;
};
}
#endif
