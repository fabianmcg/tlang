#include <Parallel/Pipeline.hh>
#include <Parallel/CreateParallelContextHierarchy.hh>
#include <Parallel/CreateConstructDatabase.hh>
#include <Parallel/GenerateConstructs.hh>
#include <Parallel/AnalyzeRegions.hh>
#include <Parallel/TransformLoops.hh>

namespace tlang {
ParallelPipeline ParallelPipeline::createPipeline(CompilerInvocation &CI) {
  ParallelPipeline pipeline(CI);
  UnitPM unitPM;
  unitPM.addPass(CreateParallelContextHierarchy(CI));
  unitPM.addPass(CreateConstructDatabase());
  unitPM.addPass(AnalyzeRegions(CI));
  unitPM.addPass(GenerateConstructs(CI));
  unitPM.addPass(TransformLoops(CI));
  pipeline.addPass(impl::makePassAdaptor<UnitDecl, UniversePM>(std::move(unitPM)));
  return pipeline;
}
}
