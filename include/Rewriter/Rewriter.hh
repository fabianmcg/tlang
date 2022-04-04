#ifndef REWRITER_REWRITER_HH
#define REWRITER_REWRITER_HH

#include <AST/ASTContext.hh>

namespace tlang {
class Rewriter {
public:
  Rewriter(ASTContext &context);
  void run();
private:
  ASTContext &context;
};
}

#endif
