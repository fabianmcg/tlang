#ifndef SEMA_SEMA_HH
#define SEMA_SEMA_HH

#include <Frontend/CompilerInvocation.hh>

namespace tlang {
class Sema {
public:
  Sema(CompilerInvocation &context);
  void run();
  static void resolveNames(ASTNode *node);
  static void resolveSymbolTables(UnitDecl *node);
private:
  void completeTable();
  void resolveTypes();
  void resolveNames();
  void inferTypes();
  CompilerInvocation &context;
};
}
#endif
