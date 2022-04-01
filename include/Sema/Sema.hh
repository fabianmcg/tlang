#ifndef SEMA_SEMA_HH
#define SEMA_SEMA_HH

#include <AST/ASTContext.hh>
#include "SymbolTablePass.hh"
#include <Sema/FirstSemaPass.hh>
//#include <Sema/TypeInference.hh>
//#include "Parent.hh"

namespace tlang::sema {
struct Sema {
  Sema(ASTContext &context) :
      context(context) {
    SymbolTablePass(context);
  }
  void analyze() {
    FirstSemaPass(context);
//    TypeInferenceSemaPass(context);
  }
  ASTContext &context;
};
}
#endif
