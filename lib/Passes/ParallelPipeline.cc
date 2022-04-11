#include <Passes/Passes/ParallelPipeline.hh>
#include <Passes/PassManager.hh>
#include "ParallelPipeline/AddGpuFunctions.hh"

namespace tlang {
void ParallelPipeline::init() {
  ModulePM MPM;
  MPM.addPass(AddGPUFunctions(context));
  addPass(impl::makePassAdaptor<ModuleDecl, ProgramPM>(std::move(MPM)));
}
PassID* ParallelPipeline::ID() {
  static PassID pid { };
  return &pid;
}
}
