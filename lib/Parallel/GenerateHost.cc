#include <Parallel/GenerateConstructs.hh>
#include <AST/Api.hh>
#include <AST/Visitors/ASTVisitor.hh>
#include <Io/CXX.hh>

namespace tlang {
void GenerateConstructs::addHostAPI() {
  ASTApi builder { CI.getContext() };
  builder.AddToContext(APIModule,builder.CreateExternFunction("__tlang_host_sync", QualType()));
  builder.AddToContext(APIModule,builder.CreateExternFunction("__tlang_host_tid", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)));
  builder.AddToContext(APIModule,builder.CreateExternFunction("__tlang_host_dim", builder.CreateType<IntType>(IntType::P_32, IntType::Signed)));
}
void GenerateConstructs::generateHostRegion(ConstructData<ParallelStmt> region) {
  ASTApi builder { CI.getContext() };
  auto fn = generateRegion(region, ContextStmt::Host);
  builder.AddToContext(hostModule, fn);
  regions[region.construct.node] = fn;
  region.construct.reference.makeNull<Stmt>();
  incrementRegionLabel(region.functor);
}

void GenerateConstructs::generateHostLaunch(ConstructData<ParallelStmt> region) {
  ASTApi builder { CI.getContext() };
  List<Expr*> arguments;
  auto cs = contextCS[dyn_cast<ContextStmt>(region.construct.node->getContext().data())];
  assert(cs);
  size_t i = 0;
  if (region.construct.node->getLaunchParameters().size()) {
    auto te = region.construct.node->getLaunchParameters()[0];
    if (te->getElements().size()) {
      auto ex = te->getElements()[0];
      arguments.push_back(ex);
      ++i;
    }
  }
  if (i == 0)
    arguments.push_back(builder.CreateLiteral((int64_t) 0, IntType::P_32));
  if (auto opts = region.construct.node->getParallelOptions()) {
    for (auto &expr : opts->getFirstPrivateVariables())
      arguments.push_back(expr);
    for (auto &expr : opts->getSharedVariables())
      arguments.push_back(builder.CreateUnOp(UnaryOperator::Address, expr));
  }
  ExternFunctionDecl *lfn = launchFunctions[region.construct.node];
  FunctionDecl *rfn = regions[region.construct.node];
  auto cxxFn = CXXIOVisitor { }.emitExternFunctionDecl(lfn);
  auto cxxKernel = CXXIOVisitor { }.emitExternFunctorDecl(rfn) + ";\n";
  {
    std::string args = frmt("num_threads, reinterpret_cast<void*>({})", rfn->getIdentifier());
    auto sz = lfn->getParameters().size();
    for (auto [i, arg] : tlang::enumerate(lfn->getParameters())) {
      if (i > 0)
        args += frmt(", {}", arg->getIdentifier());
    }
    cxxFn = frmt("{} {{\n  __tlang_host_run_kernel_rt({});\n}}\n", cxxFn, args);
  }
  cxxModule->add(builder.CreateCXX(cxxKernel));
  cxxModule->add(builder.CreateCXX(cxxFn));
  assert(lfn);
  cs->addStmt(builder.CreateCallExpr(builder.CreateDeclRefExpr(lfn), std::move(arguments)));
}
}
