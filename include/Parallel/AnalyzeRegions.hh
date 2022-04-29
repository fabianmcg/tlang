#ifndef PARALLEL_ANALYZEREGIONS_HH
#define PARALLEL_ANALYZEREGIONS_HH

#include <AST/Visitors/ASTVisitor.hh>
#include <Parallel/CreateConstructDatabase.hh>

namespace tlang {
class AnalyzeRegions: public CompilerInvocationRef, public PassBase<AnalyzeRegions> {
public:
  using CompilerInvocationRef::CompilerInvocationRef;
  bool run(UnitDecl &decl, AnyASTNodeRef ref, ResultManager &results);
private:
  std::set<ParallelConstruct*> visited;
};
}

#endif
