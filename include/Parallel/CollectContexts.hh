//#ifndef PARALLEL_COLLECTCONTEXTS_HH
//#define PARALLEL_COLLECTCONTEXTS_HH
//
//#include <deque>
//#include <AST/Visitors/EditVisitor.hh>
//#include <Parallel/Pipeline.hh>
//#include <Support/Format.hh>
//
//namespace tlang {
//struct ContextList: public ResultConcept {
//  std::list<std::pair<ContextStmt*, AnyASTNodeRef>> hostContexts;
//  std::list<std::pair<ContextStmt*, AnyASTNodeRef>> deviceContexts;
//};
//struct CollectParallelContexts: PassBase<CollectParallelContexts> {
//  struct Visitor: EditVisitor<Visitor, VisitorPattern::preOrder> {
//    Visitor(ContextList &contexts) :
//        contexts(contexts) {
//    }
//    visit_t visitContextStmt(ContextStmt *stmt, AnyASTNodeRef ref) {
//      if (stmt->getContextKind() == ContextStmt::Device)
//        contexts.deviceContexts.push_back( { stmt, ref });
//      else
//        contexts.hostContexts.push_back( { stmt, ref });
//      return visit;
//    }
//    ContextList &contexts;
//  };
//  bool run(UnitDecl &decl, AnyASTNodeRef ref, ResultManager &results) {
//    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::yellow_green), "Collecting parallel contexts\n");
//    ContextList contexts;
//    Visitor { contexts }.traverseUnitDecl(&decl, ref);
//    size_t ndc = contexts.deviceContexts.size();
//    size_t nhc = contexts.hostContexts.size();
//    results.addResult(ID(), &decl, new ContextList(std::move(contexts)));
//    print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::lime_green),
//        "Finished collecting parallel contexts, there are: [H: {}, D: {}] contexts.\n", nhc, ndc);
//    return true;
//  }
//  static PassID* ID() {
//    static PassID pid { };
//    return &pid;
//  }
//};
//}
//
//#endif
