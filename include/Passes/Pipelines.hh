#ifndef PASSES_PIPELINES_HH
#define PASSES_PIPELINES_HH

#include <Frontend/CompilerInvocation.hh>
#include <Passes/PassManager.hh>

namespace tlang {
class MainPipeline: public CompilerInvocationRef, public PassBase<MainPipeline>, public UniversePM {
public:
  using CompilerInvocationRef::CompilerInvocationRef;
  using UniversePM::run;
  static llvm::StringRef name() {
    return "MainPipeline";
  }
  static MainPipeline createPipeline(CompilerInvocation &CI);
  static int run(CompilerInvocation &CI);
protected:
  ResultManager results { };
};

class TransformationPipeline: public CompilerInvocationRef, public PassBase<TransformationPipeline>, public UnitPM {
public:
  using CompilerInvocationRef::CompilerInvocationRef;
  static llvm::StringRef name() {
    return "TransformationPipeline";
  }
  static TransformationPipeline createPipeline(CompilerInvocation &CI);
};
}

#endif
