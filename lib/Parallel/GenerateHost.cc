#include <Parallel/GenerateConstructs.hh>
#include <AST/Api.hh>
#include <AST/Visitors/ASTVisitor.hh>

namespace tlang {
void GenerateConstructs::addHostAPI() {
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
  if (auto opts = region.construct.node->getParallelOptions()) {
    for (auto &expr : opts->getFirstPrivateVariables())
      arguments.push_back(expr);
    for (auto &expr : opts->getSharedVariables())
      arguments.push_back(builder.CreateUnOp(UnaryOperator::Address, expr));
  }
  ExternFunctionDecl *lfn = launchFunctions[region.construct.node];
  assert(lfn);
  AnyASTNodeRef ref = implicitRefs[dyn_cast<ImplicitContextStmt>(region.construct.node->getContext().data())];
  assert(ref);
  ref.assign<Stmt>(builder.CreateCallExpr(builder.CreateDeclRefExpr(lfn), std::move(arguments)));
}
}
