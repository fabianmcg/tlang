#ifndef PARALLEL_CREATECONSTRUCTDATABASE_HH
#define PARALLEL_CREATECONSTRUCTDATABASE_HH

#include <deque>
#include <AST/Visitors/EditVisitor.hh>
#include <Parallel/Pipeline.hh>
#include <Support/Format.hh>
#include <Parallel/ParallelConstructDatabase.hh>

namespace tlang {
struct CreateConstructDatabase: PassBase<CreateConstructDatabase> {
  struct Visitor: EditVisitor<Visitor, VisitorPattern::prePostOrder> {
    Visitor(ParallelConstructDatabase &contexts) :
        contexts(contexts) {
    }
    visit_t visitFunctorDecl(FunctorDecl *node, AnyASTNodeRef ref, VisitType kind) {
      managFunctionStack(node, kind);
      return visit;
    }
    visit_t visitContextStmt(ContextStmt *stmt, AnyASTNodeRef ref, VisitType kind) {
      if (kind == postVisit)
        return visit;
      contexts.contexts.push_back(ConstructData<ContextStmt> { stmt, ref, functionStack.front() });
      return visit;
    }
    visit_t visitParallelStmt(ParallelStmt *stmt, AnyASTNodeRef ref, VisitType kind) {
      if (kind == postVisit)
        return visit;
      if (auto cs = dyn_cast<ContextStmt>(stmt->getContext().data())) {
        if (cs->getContextKind() == ContextStmt::Device)
          contexts.deviceRegions.push_back(ConstructData<ParallelStmt> { stmt, ref, functionStack.front() });
        else if (cs->getContextKind() == ContextStmt::Host)
          contexts.hostRegions.push_back(ConstructData<ParallelStmt> { stmt, ref, functionStack.front() });
        else
          contexts.sequentialRegions.push_back(ConstructData<ParallelStmt> { stmt, ref, functionStack.front() });
      } else
        throw(std::runtime_error("Invalid parallel construct"));
      return visit;
    }
    visit_t managFunctionStack(FunctorDecl *node, VisitType kind) {
      if (kind == preVisit)
        functionStack.push_front(node);
      else if (functionStack.size())
        functionStack.pop_front();
      return visit;
    }
    ParallelConstructDatabase &contexts;
    std::deque<FunctorDecl*> functionStack;
  };
  bool run(UnitDecl &decl, AnyASTNodeRef ref, ResultManager &results) {
    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::yellow_green), "Collecting parallel contexts\n");
    ParallelConstructDatabase contexts;
    Visitor { contexts }.traverseUnitDecl(&decl, ref);
    size_t nc = contexts.contexts.size();
    size_t nsc = contexts.sequentialRegions.size();
    size_t nhc = contexts.hostRegions.size();
    size_t ndc = contexts.deviceRegions.size();
    if (!contexts.empty())
      results.addResult(ID(), &decl, new ParallelConstructDatabase(std::move(contexts)));
    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::lime_green),
        "Finished collecting parallel contexts, there are: [C: {}, S: {}, H: {}, D: {}] contexts.\n", nc, nsc, nhc, ndc);
    return true;
  }
  static PassID* ID() {
    static PassID pid { };
    return &pid;
  }
};
}

#endif
