#ifndef PARALLEL_CREATECONSTRUCTDATABASE_HH
#define PARALLEL_CREATECONSTRUCTDATABASE_HH

#include <deque>
#include <AST/Visitors/EditVisitor.hh>
#include <Parallel/Pipeline.hh>
#include <Support/Format.hh>

namespace tlang {
struct ParallelConstructDatabase: public ResultConcept {
  std::list<ASTData<ContextStmt>> contexts;
  std::list<ASTData<ParallelStmt>> hostRegions;
  std::list<ASTData<ParallelStmt>> deviceRegions;
};
struct CreateConstructDatabase: PassBase<CreateConstructDatabase> {
  struct Visitor: EditVisitor<Visitor, VisitorPattern::preOrder> {
    Visitor(ParallelConstructDatabase &contexts) :
        contexts(contexts) {
    }
    visit_t visitContextStmt(ContextStmt *stmt, AnyASTNodeRef ref) {
      contexts.contexts.push_back(ASTData<ContextStmt> { stmt, ref });
      return visit;
    }
    visit_t visitParallelStmt(ParallelStmt *stmt, AnyASTNodeRef ref) {
      if (auto cs = dyn_cast<ContextStmt>(stmt->getContext().data())) {
        if (cs->getContextKind() == ContextStmt::Device)
          contexts.deviceRegions.push_back(ASTData<ParallelStmt> { stmt, ref });
        else
          contexts.hostRegions.push_back(ASTData<ParallelStmt> { stmt, ref });
      } else
        throw(std::runtime_error("Invalid parallel construct"));
      return visit;
    }
    ParallelConstructDatabase &contexts;
  };
  bool run(UnitDecl &decl, AnyASTNodeRef ref, ResultManager &results) {
    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::yellow_green), "Collecting parallel contexts\n");
    ParallelConstructDatabase contexts;
    Visitor { contexts }.traverseUnitDecl(&decl, ref);
    size_t nc = contexts.contexts.size();
    size_t nhc = contexts.hostRegions.size();
    size_t ndc = contexts.deviceRegions.size();
    results.addResult(ID(), &decl, new ParallelConstructDatabase(std::move(contexts)));
    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::lime_green),
        "Finished collecting parallel contexts, there are: [C: {}, H: {}, D: {}] contexts.\n", nc, nhc, ndc);
    return true;
  }
  static PassID* ID() {
    static PassID pid { };
    return &pid;
  }
};
}

#endif
