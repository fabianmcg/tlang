//#ifndef PARALLEL_TRANSFORMCONTEXTS_HH
//#define PARALLEL_TRANSFORMCONTEXTS_HH
//
//#include <Parallel/CollectContexts.hh>
//
//namespace tlang {
//struct TransformContexts: public CompilerInvocationRef, PassBase<TransformContexts> {
//  using CompilerInvocationRef::CompilerInvocationRef;
//  void removeContexts(ContextList &contexts);
//  bool run(UnitDecl &decl, AnyASTNodeRef ref, ResultManager &results) {
//    auto contexts = results.getResult<ContextList>(CollectParallelContexts::ID(), &decl);
//    if (contexts) {
//      print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::yellow_green), "Transforming parallel contexts\n");
//
//      print(std::cerr, fmt::emphasis::bold | fmt::fg(fmt::color::lime_green), "Finished transforming parallel contexts\n");
//    }
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
