#ifndef PASSES_PARALLELPIPELINE_ANALYSIS_HH
#define PASSES_PARALLELPIPELINE_ANALYSIS_HH

#include <Passes/Passes/ParallelPipeline.hh>
#include <AST/Visitors/ASTVisitor.hh>

namespace tlang {
struct ParallelAnalysis: ResultConcept {
  ASTKind kind;
  std::list<Stmt*> parallelSections;
};
struct ParallelAnalysisPass: public ASTContextReference, public PassBase<ParallelAnalysisPass> {
  struct Visitor: public ASTVisitor<Visitor, VisitorPattern::preOrder> {
    Visitor(ResultManager &manager) :
        manager(manager) {
    }

    ResultManager &manager;
  };
  using ASTContextReference::ASTContextReference;
  bool run(ModuleDecl &module, AnyASTNodeRef nodeRef, ResultManager &manager) {
    Visitor { manager }.traverseModuleDecl(&module);
    return true;
  }
  static PassID* ID() {
    static PassID pid { };
    return &pid;
  }
};
}

#endif
