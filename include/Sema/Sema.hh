#ifndef __SEMA_SEMA_HH__
#define __SEMA_SEMA_HH__

#include "AST/Include.hh"
#include "NameResolution.hh"

namespace tlang::sema {
struct Sema {
  Sema(ASTContext &context) :
      context(context) {
  }
  void name_resolution() {
    NameResolution resolve(context);
    resolve.analyze();
  }
  void analyze() {
    name_resolution();
  }
  ASTContext &context;
};
}
#endif
