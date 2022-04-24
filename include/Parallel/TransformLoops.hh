#ifndef PARALLEL_TRANSFORMLOOPS_HH
#define PARALLEL_TRANSFORMLOOPS_HH

#include <Parallel/CreateConstructDatabase.hh>

namespace tlang {
class TransformLoops: public CompilerInvocationRef, public PassBase<TransformLoops> {
public:
  using CompilerInvocationRef::CompilerInvocationRef;
  bool run(UnitDecl &decl, AnyASTNodeRef ref, ResultManager &results) {
    return true;
  }
};
}

#endif
