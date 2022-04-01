#ifndef SEMA_SEMA_HH
#define SEMA_SEMA_HH

#include <AST/ASTContext.hh>

namespace tlang {
class Sema {
public:
  Sema(ASTContext &context);
  void run();
private:
  void completeTable();
  void resolveTypes();
  void resolveNames();
  void inferTypes();
  ASTContext &context;
};
}
#endif
