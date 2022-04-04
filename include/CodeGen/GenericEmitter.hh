#ifndef CODEGEN_GENERICEMITTER_HH
#define CODEGEN_GENERICEMITTER_HH

#include <deque>
#include <CodeGen/CodeEmitter.hh>

namespace tlang::codegen {
class GenericEmitter: public CodeEmitter {
public:
  using CodeEmitter::CodeEmitter;

  virtual void run(UnitDecl *unit);
};
}

#endif
