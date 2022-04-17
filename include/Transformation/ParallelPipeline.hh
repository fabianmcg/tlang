#ifndef PASSES_PASSES_PARALLELPIPELINE_HH
#define PASSES_PASSES_PARALLELPIPELINE_HH

#include <AST/ASTContext.hh>
#include <Passes/Impl/PassManager.hh>
#include <Passes/ResultManager.hh>

namespace tlang {
struct ParallelPipeline: public ASTContextReference, public PassBase<ParallelPipeline>, public impl::PassManager<UniverseDecl, ResultManager, bool> {
  using ASTContextReference::ASTContextReference;
  static llvm::StringRef name() {
    return "ParallelPipeline";
  }
  void init();
  static PassID* ID();
};
}

#endif
