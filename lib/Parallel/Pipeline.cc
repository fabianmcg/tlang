#include <Parallel/CreateConstructDatabase.hh>
#include <Parallel/Pipeline.hh>
#include <Parallel/CreateParallelContextHierarchy.hh>
#include <Parallel/TransformContexts.hh>

namespace tlang {
ParallelPipeline ParallelPipeline::createPipeline(CompilerInvocation &CI) {
  ParallelPipeline pipeline(CI);
  UnitPM unitPM;
  unitPM.addPass(CreateParallelContextHierarchy(CI));
  unitPM.addPass(CreateConstructDatabase());
//  unitPM.addPass(TransformContexts(CI));
  pipeline.addPass(impl::makePassAdaptor<UnitDecl, UniversePM>(std::move(unitPM)));
  return pipeline;
}
}
