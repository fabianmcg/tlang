#include <Passes/Pipelines.hh>
#include <Parallel/Pipeline.hh>
#include <Transformation/SimplifyExpr.hh>
#include <Transformation/AddImplicit.hh>

namespace tlang {
MainPipeline MainPipeline::createPipeline(CompilerInvocation &CI) {
  MainPipeline pipeline(CI);
  pipeline.addPass(impl::makePassAdaptor<UnitDecl, UniversePM>(TransformationPipeline::createPipeline(CI)));
  pipeline.addPass(ParallelPipeline::createPipeline(CI));
  pipeline.addPass(impl::makePassAdaptor<UnitDecl, UniversePM>(LoweringPipeline::createPipeline(CI)));
  return pipeline;
}

int MainPipeline::run(CompilerInvocation &CI) {
  MainPipeline pipeline = createPipeline(CI);
  pipeline.run(**pipeline.CI.getContext(), AnyASTNodeRef { }, pipeline.results);
  return 0;
}

TransformationPipeline TransformationPipeline::createPipeline(CompilerInvocation &CI) {
  TransformationPipeline pipeline(CI);
  ExprPM EPM;
  EPM.addPass(SimplifyExpr { CI.getContext() });
  pipeline.addPass(impl::makePassAdaptor<Expr, UnitPM>(std::move(EPM)));
  return pipeline;
}

LoweringPipeline LoweringPipeline::createPipeline(CompilerInvocation &CI) {
  LoweringPipeline pipeline(CI);
  ModulePM EPM;
  EPM.addPass(AddImplicitExpr { CI.getContext() });
  pipeline.addPass(impl::makePassAdaptor<ModuleDecl, UnitPM>(std::move(EPM)));
  return pipeline;
}
}
