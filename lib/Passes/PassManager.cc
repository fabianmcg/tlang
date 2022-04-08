#include <Passes/PassManager.hh>

namespace tlang {
template class impl::PassManager<ProgramDecl, ResultManager, bool>;
template class impl::PassManager<UnitDecl, ResultManager, bool>;
template class impl::PassManager<ModuleDecl, ResultManager, bool>;
template class impl::PassManager<Decl, ResultManager, bool>;
template class impl::PassManager<FunctorDecl, ResultManager, bool>;
template class impl::PassManager<Stmt, ResultManager, bool>;
template class impl::PassManager<Expr, ResultManager, bool>;

struct FunctionName: public PassBase<FunctionName> {
  bool run(FunctorDecl &decl, ResultManager &manager) {
    std::cerr << decl.getIdentifier() << std::endl;
    return true;
  }
};

int PassManager::run() {
  FunctorDeclPM FPM;
  FPM.addPass(FunctionName());
  program_manager.addPass(impl::makePassAdaptor<FunctorDecl, ProgramPM>(std::move(FPM)));
  program_manager.run(**context, results);
  return 0;
}
}
