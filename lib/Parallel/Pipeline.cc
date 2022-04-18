#include <Parallel/Pipeline.hh>
#include <Parallel/PrepareParallelContexts.hh>
#include <Parallel/CollectContexts.hh>
#include <Parallel/TransformContexts.hh>

namespace tlang {
ParallelPipeline ParallelPipeline::createPipeline(CompilerInvocation &CI) {
  ParallelPipeline pipeline(CI);
  UnitPM unitPM;
  unitPM.addPass(PrepareParallelContexts(CI));
  unitPM.addPass(CollectParallelContexts());
  unitPM.addPass(TransformContexts(CI));
  pipeline.addPass(impl::makePassAdaptor<UnitDecl, UniversePM>(std::move(unitPM)));
  return pipeline;
}
}
