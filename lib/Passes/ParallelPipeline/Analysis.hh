#ifndef PASSES_PARALLELPIPELINE_ANALYSIS_HH
#define PASSES_PARALLELPIPELINE_ANALYSIS_HH

#include <Passes/Passes/ParallelPipeline.hh>
#include <AST/Visitors/ASTVisitor.hh>

namespace tlang {
struct ParallelAnalysis: ResultConcept {
  struct SectionAnalysis {
    SectionAnalysis(AnyASTNodeRef ref, Stmt *section) :
        ref(ref), section(section) {
    }
    AnyASTNodeRef ref;
    Stmt *section;
    std::list<std::pair<AnyASTNodeRef, VariableDecl>> sharedVariables;
  };
  std::list<SectionAnalysis> parallelSections;
};
struct ParallelAnalysisPass: public ASTContextReference, public PassBase<ParallelAnalysisPass> {
  struct Visitor: public EditVisitor<Visitor, VisitorPattern::preOrder> {
    Visitor(ModuleDecl *module, ResultManager &manager) :
        module(module), manager(manager) {
    }
    visit_t visitBlockStmt(BlockStmt *block, AnyASTNodeRef nodeRef) {
      auto analysis = new ParallelAnalysis { };
      analysis->parallelSections.push_back( { nodeRef, block });
      manager.addResult(ParallelAnalysisPass::ID(), module, analysis);
      return visit;
    }
    ModuleDecl *module;
    ResultManager &manager;
  };
  using ASTContextReference::ASTContextReference;
  bool run(ModuleDecl &module, AnyASTNodeRef nodeRef, ResultManager &manager) {
    Visitor { &module, manager }.traverseModuleDecl(&module, nodeRef);
    return true;
  }
  static PassID* ID() {
    static PassID pid { };
    return &pid;
  }
};
}

#endif
