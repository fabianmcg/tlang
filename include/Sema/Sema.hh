#ifndef SEMA_SEMA_HH
#define SEMA_SEMA_HH

#include <Frontend/CompilerInvocation.hh>

namespace tlang {
class Sema {
public:
  Sema(CompilerInvocation &context);
  void run();
  void resolveNames(ASTNode * node, bool setType = false);
private:
  void completeTable();
  void resolveTypes();
  void resolveNames();
  void inferTypes();
  CompilerInvocation &context;
};
}
#endif
