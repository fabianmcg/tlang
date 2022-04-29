#ifndef FRONTEND_COMPILERINVOCATION_HH
#define FRONTEND_COMPILERINVOCATION_HH

#include <Frontend/CompilerOptions.hh>
#include <AST/ASTContext.hh>

namespace tlang {
class CompilerInvocation {
public:
  CompilerInvocation(CompilerOptions options = { }) :
      options(options) {
  }
  const CompilerOptions& getOptions() const {
    return options;
  }
  ASTContext& operator*() {
    return context;
  }
  const ASTContext& operator*() const {
    return context;
  }
  ASTContext& getContext() {
    return context;
  }
  const ASTContext& getContext() const {
    return context;
  }
protected:
  CompilerOptions options;
  ASTContext context { };
};
struct CompilerInvocationRef {
  CompilerInvocationRef(CompilerInvocation &CI) :
      CI(CI) {
  }
  CompilerInvocation &CI;
};
}

#endif
