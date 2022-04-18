#include <Passes/PassManager.hh>

namespace tlang {
template class impl::PassManager<UniverseDecl, ResultManager, bool>;
template class impl::PassManager<UnitDecl, ResultManager, bool>;
template class impl::PassManager<ModuleDecl, ResultManager, bool>;
template class impl::PassManager<Decl, ResultManager, bool>;
template class impl::PassManager<FunctorDecl, ResultManager, bool>;
template class impl::PassManager<Stmt, ResultManager, bool>;
template class impl::PassManager<Expr, ResultManager, bool>;
}
