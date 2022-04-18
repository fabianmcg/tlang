#ifndef PARALLEL_PIPELINE_HH
#define PARALLEL_PIPELINE_HH

#include <Frontend/CompilerInvocation.hh>
#include <Passes/PassManager.hh>

namespace tlang {
class ParallelPipeline: public CompilerInvocationRef, public PassBase<ParallelPipeline>, public UniversePM {
public:
  using CompilerInvocationRef::CompilerInvocationRef;
  static llvm::StringRef name() {
    return "ParallelPipeline";
  }
  static ParallelPipeline createPipeline(CompilerInvocation &CI);
};
}

#endif
