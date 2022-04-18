#ifndef PASSES_PASSMANAGER_HH
#define PASSES_PASSMANAGER_HH

#include <vector>
#include <Frontend/CompilerInvocation.hh>
#include <Passes/Impl/PassManager.hh>
#include <Passes/ResultManager.hh>

namespace tlang {
extern template class impl::PassManager<UniverseDecl, ResultManager, bool>;
using UniversePM = impl::PassManager<UniverseDecl, ResultManager, bool>;

extern template class impl::PassManager<UnitDecl, ResultManager, bool>;
using UnitPM = impl::PassManager<UnitDecl, ResultManager, bool>;

extern template class impl::PassManager<ModuleDecl, ResultManager, bool>;
using ModulePM = impl::PassManager<ModuleDecl, ResultManager, bool>;

extern template class impl::PassManager<Decl, ResultManager, bool>;
using DeclPM = impl::PassManager<Decl, ResultManager, bool>;

extern template class impl::PassManager<FunctorDecl, ResultManager, bool>;
using FunctorPM = impl::PassManager<FunctorDecl, ResultManager, bool>;

extern template class impl::PassManager<Stmt, ResultManager, bool>;
using StmtPM = impl::PassManager<Stmt, ResultManager, bool>;

extern template class impl::PassManager<Expr, ResultManager, bool>;
using ExprPM = impl::PassManager<Expr, ResultManager, bool>;
}

#endif
