#ifndef PASSES_PASSMANAGER_HH
#define PASSES_PASSMANAGER_HH

#include <vector>
#include <AST/ASTContext.hh>
#include <Passes/Impl/PassManager.hh>
#include <Passes/ResultManager.hh>

namespace tlang {
extern template class impl::PassManager<UniverseDecl, ResultManager, bool>;
using ProgramPM = impl::PassManager<UniverseDecl, ResultManager, bool>;

extern template class impl::PassManager<UnitDecl, ResultManager, bool>;
using UnitPM = impl::PassManager<UnitDecl, ResultManager, bool>;

extern template class impl::PassManager<ModuleDecl, ResultManager, bool>;
using ModulePM = impl::PassManager<ModuleDecl, ResultManager, bool>;

extern template class impl::PassManager<Decl, ResultManager, bool>;
using DeclPM = impl::PassManager<Decl, ResultManager, bool>;

extern template class impl::PassManager<FunctorDecl, ResultManager, bool>;
using FunctorDeclPM = impl::PassManager<FunctorDecl, ResultManager, bool>;

extern template class impl::PassManager<Stmt, ResultManager, bool>;
using StmtPM = impl::PassManager<Stmt, ResultManager, bool>;

extern template class impl::PassManager<Expr, ResultManager, bool>;
using ExprPM = impl::PassManager<Expr, ResultManager, bool>;

class PassManager {
public:
  PassManager(ASTContext &context) :
      context(context) {
  }
  int run();
  void registerDefaultPipeline();
  template <typename T>
  void appendPass(T &&pass) {
    program_manager.addPass(std::move(pass));
  }
protected:
  ASTContext &context;
  ResultManager results;
  ProgramPM program_manager;
};
}

#endif
