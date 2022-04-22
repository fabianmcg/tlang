#include <Parallel/Pipeline.hh>
#include <Parallel/CreateParallelContextHierarchy.hh>
#include <Parallel/CreateConstructDatabase.hh>
#include <Parallel/GenerateConstructs.hh>

namespace tlang {
ParallelPipeline ParallelPipeline::createPipeline(CompilerInvocation &CI) {
  ParallelPipeline pipeline(CI);
  UnitPM unitPM;
  unitPM.addPass(CreateParallelContextHierarchy(CI));
  unitPM.addPass(CreateConstructDatabase());
  unitPM.addPass(GenerateConstructs(CI));
  pipeline.addPass(impl::makePassAdaptor<UnitDecl, UniversePM>(std::move(unitPM)));
  return pipeline;
}
}
