#include <Transformation/ParallelPipeline.hh>
#include <Transformation/SimplifyExpr.hh>
#include <Passes/PassManager.hh>

namespace tlang {
template class impl::PassManager<ProgramDecl, ResultManager, bool>;
template class impl::PassManager<UnitDecl, ResultManager, bool>;
template class impl::PassManager<ModuleDecl, ResultManager, bool>;
template class impl::PassManager<Decl, ResultManager, bool>;
template class impl::PassManager<FunctorDecl, ResultManager, bool>;
template class impl::PassManager<Stmt, ResultManager, bool>;
template class impl::PassManager<Expr, ResultManager, bool>;

void PassManager::registerDefaultPipeline() {
  FunctorDeclPM FPM;
  ExprPM EPM;
  EPM.addPass(SimplifyExpr { context });
  program_manager.addPass(impl::makePassAdaptor<FunctorDecl, ProgramPM>(std::move(FPM)));
  program_manager.addPass(impl::makePassAdaptor<Expr, ProgramPM>(std::move(EPM)));
  ParallelPipeline parallelPipeline(context);
  parallelPipeline.init();
  program_manager.addPass(std::move(parallelPipeline));
}

int PassManager::run() {
  program_manager.run(**context, AnyASTNodeRef { }, results);
  return 0;
}
}
